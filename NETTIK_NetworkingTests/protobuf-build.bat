@echo off
for %%x in (*proto) do "../protobuf/protoc.exe" --cpp_out="./" "%%x"
echo Command complete, check console for errors.
pause