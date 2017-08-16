rem - Use: Developer Command Prompt VS2015

rmdir /S /Q bin
mkdir bin

cd wic_library
MSBuild WIC_Decoder.sln /t:Rebuild /p:Configuration=Release /p:Platform=x86
cd ..

cd wic_plugin
MSBuild WIC_Decoder.sln /t:Rebuild /p:Configuration=Release /p:Platform=x86
cd ..

copy C:\gimp-sdk\bin\msvcp120.dll bin\Release /Y
copy C:\gimp-sdk\bin\msvcr120.dll bin\Release /Y

cd bin
cd Release
7za a -tzip WIC_Plugin.zip *
cd ..
cd ..