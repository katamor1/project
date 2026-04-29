@echo off
setlocal
copy /b skills_zip_part_01.bin+skills_zip_part_02.bin+skills_zip_part_03.bin+skills_zip_part_04.bin+skills_zip_part_05.bin+skills_zip_part_06.bin+skills_zip_part_07.bin+skills_zip_part_08.bin+skills_zip_part_09.bin skills.zip
certutil -hashfile skills.zip SHA256
echo Expected SHA256:
echo 69f0ec3de43813484d9efe396c6eea4bea7db92ea213a5a6becbc19e118ab7db
