$Qt_ROOT_DIR=$env:Qt6_DIR
$SRC_DIR=(get-item $PSScriptRoot).parent.FullName
$BUILD_DIR=$SRC_DIR + "/build"
$PACKAGE_DIR=$SRC_DIR + "/package"
Write-Host -BackgroundColor Yellow "Qt_ROOT_DIR: $Qt_ROOT_DIR"
Write-Host -BackgroundColor Yellow "BUILD_DIR: $BUILD_DIR"
Write-Host -BackgroundColor Yellow "PACKAGE_DIR: $PACKAGE_DIR"

# ------------------------------------------------------------------------------
Write-Host -BackgroundColor Yellow "CLEANING"
if (Test-Path $BUILD_DIR) {Remove-Item -LiteralPath $BUILD_DIR -Force -Recurse}
if (Test-PATH $PACKAGE_DIR) {Remove-Item -LiteralPath $PACKAGE_DIR -Force -Recurse}

# ------------------------------------------------------------------------------
Write-Host -BackgroundColor Yellow "BUILDING"
cmake -S $SRC_DIR -B $BUILD_DIR -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$Qt_ROOT_DIR
ninja -C $BUILD_DIR

# ------------------------------------------------------------------------------
Write-Host -BackgroundColor Yellow "PACKAGING"
# 0 - prepare dir
cd $SRC_DIR
New-Item -ItemType "directory" -Path $PACKAGE_DIR

# 1 - copy gotify-tray++ build
Copy-Item $BUILD_DIR/gotify-tray++.exe -Destination $PACKAGE_DIR

# 2 - copy dlls
# windeployqt $PACKAGE_DIR/gotify-tray++.exe --no-translations --no-system-d3d-compiler --no-opengl-sw
Copy-Item $Qt_ROOT_DIR/bin/Qt6Core.dll           -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/Qt6Gui.dll            -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/Qt6Network.dll        -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/Qt6Sql.dll            -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/Qt6WebSockets.dll     -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/Qt6Widgets.dll        -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/libgcc_s_seh-1.dll    -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/libstdc++-6.dll       -Destination $PACKAGE_DIR
Copy-Item $Qt_ROOT_DIR/bin/libwinpthread-1.dll   -Destination $PACKAGE_DIR

Copy-Item -Recurse $Qt_ROOT_DIR/plugins/iconengines  -Exclude *.debug -Destination $PACKAGE_DIR
Copy-Item -Recurse $Qt_ROOT_DIR/plugins/imageformats -Exclude *.debug -Destination $PACKAGE_DIR
Copy-Item -Recurse $Qt_ROOT_DIR/plugins/styles       -Exclude *.debug -Destination $PACKAGE_DIR
Copy-Item -Recurse $Qt_ROOT_DIR/plugins/tls          -Exclude *.debug -Destination $PACKAGE_DIR

New-Item -ItemType "directory" -Path $PACKAGE_DIR/platforms
Copy-Item $Qt_ROOT_DIR/plugins/platforms/qwindows.dll -Destination $PACKAGE_DIR/platforms
New-Item -ItemType "directory" -Path  $PACKAGE_DIR/sqldrivers
Copy-Item $Qt_ROOT_DIR/plugins/sqldrivers/qsqlite.dll -Destination $PACKAGE_DIR/sqldrivers

Get-ChildItem $PACKAGE_DIR
Write-Host -BackgroundColor Green "PACKAGING DONE"
