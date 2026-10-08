@set MAPATH="drive:\path\to\mylevel.map"
@set Q3MAP2="drive:\path\to\q3map2.exe" -fs_basepath "drive:\path\to\game\folder" -v -fs_game mymod

::(omit '-fs_game mymod', if using only baseq3 for mapping)

%Q3MAP2% -meta %MAPATH%
%Q3MAP2% -vis %MAPATH%
%Q3MAP2% -light -fast %MAPATH%


pause
