@echo off
echo qt = "c:\\Qt\\Qt5.6.2\\5.6\\msvc2013" >config.py

echo qt5 = 1  >>config.py
echo release = 0  >>config.py
echo debug = 1  >>config.py

setlocal ENABLEDELAYEDEXPANSION
set word=\\
set str=%CD%\3rdParty\OpenSSL\
set str=%str:\=!word!%
echo %str%

echo openssl = "%str%"  >>config.py
echo HAVE_SCHANNEL = True >> config.py
echo HAVE_OPENSSL = False >> config.py


XGConsole /command="scons -u -j16 swiften" /openmonitor /profile="incredibuild.xml"

copy %cd%\swiften\swiften.lib %cd%\swiften_debug.lib 
copy %cd%\3rdParty\Boost\Swiften_Boost.lib %cd%\3rdParty\Boost\Swiften_Boost_debug.lib

