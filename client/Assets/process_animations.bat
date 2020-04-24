@echo off

set REL_PATH=.\Animation
set ABS_PATH=

rem // Save current directory and change to target directory
pushd %REL_PATH%

rem // Save value of CD variable (current directory)
set ABS_PATH=%CD%

rem // Restore original directory
popd

.\AssetPipelineCmd -pa %ABS_PATH% %ABS_PATH%