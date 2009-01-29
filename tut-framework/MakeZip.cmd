@if "%1x" == "x" goto err

zip -r -9 %1.zip *.* -x CVS *.zip *.tgz *.tar.gz
@exit 0

:err
@echo usage: MakeZip TUT-YYYY-MM-DD
@exit 1