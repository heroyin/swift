@echo off
echo release = 1  >config.py
echo debug = 0  >>config.py
echo HAVE_LIBXML = 1 >> config.py
echo HAVE_SCHANNEL = 1 >> config.py
echo HAVE_OPENSSL = 0 >> config.py

setlocal ENABLEDELAYEDEXPANSION
set word=\\
set str=%CD%\3rdParty\OpenSSL\
set str=%str:\=!word!%
echo %str%

echo openssl = "%str%"  >>config.py
 
XGConsole /command="scons -u -j16 swiften" /openmonitor /profile="incredibuild.xml"

copy %cd%\swiften\swiften.lib %cd%\swiften_release.lib 
copy %cd%\3rdParty\Boost\Swiften_Boost.lib %cd%\3rdParty\Boost\Swiften_Boost_release.lib
copy %cd%\3rdParty\EasyLogging\EasyLogging.lib %cd%\EasyLogging_release.lib
