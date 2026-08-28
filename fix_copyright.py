import os
import subprocess

# Get all tracked files
result = subprocess.run(['git', 'ls-files'], capture_output=True, text=True)
files = result.stdout.splitlines()

for file in files:
    if not os.path.isfile(file): continue
    try:
        with open(file, 'r', encoding='utf-8') as f:
            content = f.read()
            
        new_content = content.replace('Copyright tumourlove. All Rights Reserved.', 'Copyright Thomasbehan and Contributors. All Rights Reserved.')
        new_content = new_content.replace('tumourlove', 'Thomasbehan')
        
        if new_content != content:
            with open(file, 'w', encoding='utf-8') as f:
                f.write(new_content)
    except:
        pass

print("Copyrights updated.")
