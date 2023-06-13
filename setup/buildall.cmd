@REM py -2.7 setup.py bdist_zip --plat-name=win32
@REM py -2.7 setup.py bdist_zip --plat-name=win-amd64
@REM py -2.7 setup.py bdist_wheel --plat-name=win32 --python-tag=cp27
@REM py -2.7 setup.py bdist_wheel --plat-name=win-amd64 --python-tag=cp27

@REM py -3.5 setup.py bdist_zip --plat-name=win32
@REM py -3.5 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.5 setup.py bdist_wheel --plat-name=win32 --python-tag=cp35
@REM py -3.5 setup.py bdist_wheel --plat-name=win-amd64 --python-tag=cp35

@REM py -3.6 setup.py bdist_zip --plat-name=win32
@REM py -3.6 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.6 setup.py bdist_wheel --plat-name=win32 --python-tag=cp36
@REM py -3.6 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp36

@REM py -3.7 setup.py bdist_zip --plat-name=win32
@REM py -3.7 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.7 setup.py bdist_wheel --plat-name=win32 --python-tag=cp37
@REM py -3.7 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp37

@REM py -3.8 setup.py bdist_zip --plat-name=win32
@REM py -3.8 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.8 setup.py bdist_wheel --plat-name=win32 --python-tag=cp38
@REM py -3.8 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp38

@REM py -3.9 setup.py bdist_zip --plat-name=win32
@REM py -3.9 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.9 setup.py bdist_wheel --plat-name=win32 --python-tag=cp39
@REM py -3.9 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp39

@REM py -3.11 setup.py bdist_zip --plat-name=win32
@REM py -3.11 setup.py bdist_zip --plat-name=win-amd64
@REM py -3.11 setup.py bdist_wheel --plat-name=win32 --python-tag=cp311
py -3.11 setup.py bdist_wheel --plat-name=win-amd64  --python-tag=cp311

py setup.py bdist_pdb