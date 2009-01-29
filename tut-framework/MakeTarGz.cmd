@if "%1x" == "x" goto err

C:\Work\Apps\cygwin\bin\tar c --exclude=CVS --exclude=*.tgz --exclude=*.gz --exclude=*.zip -f %1.tar *
C:\Work\Apps\cygwin\bin\gzip %1.tar
@exit 0

:err
@echo usage: MakeTarGz TUT-YYYY-MM-DD
@exit 1