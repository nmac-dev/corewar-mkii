## Compiler Options
$CXXSTD = "-std=c++17"  # C++ standard
$INC    = "-I./inc"     # Add search directory
## Files
$TARGET = "corewar"     # output file

## Create bin directory
if ( !(Test-Path .\bin) ) 
{
    New-Item -itemType Directory -Path .\bin
}

## Compile
g++ $CXXSTD $INC -o ./bin/$TARGET ./src/asm.cpp ./src/parser.cpp ./src/$TARGET.cpp

## Delete object files
Remove-Item ./bin/*.o -Recurse
## Notify
Write-Output "Success! Program can be found in the 'bin' directory"