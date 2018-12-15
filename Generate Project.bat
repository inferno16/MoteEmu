@ECHO OFF

SET defaultAction=vs2017

ECHO.
ECHO Available actions: vs2005, vs2008, vs2010, vs2012, vs2013, vs2015, vs2017, gmake, xcode4, codelite
ECHO Default action: %defaultAction%
ECHO.

SET /p option=Enter action and press enter (leave blank to use default):
ECHO.

IF [%option%] EQU [] (
    SET action=%defaultAction%
) ELSE (
    SET action=%option%
)
SET option=

CALL vendor\premake\windows\premake5.exe %action%
ECHO.

PAUSE