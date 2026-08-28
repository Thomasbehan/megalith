import os
import re
import subprocess

def git_mv(src, dst):
    subprocess.run(['git', 'mv', src, dst], check=True)

def rename_content(file_path):
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    new_content = content.replace('Monolith', 'Megalith')
    new_content = new_content.replace('monolith', 'megalith')
    new_content = new_content.replace('MONOLITH', 'MEGALITH')
    
    if new_content != content:
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)

# Get all tracked files
result = subprocess.run(['git', 'ls-files'], capture_output=True, text=True)
files = result.stdout.splitlines()

# 1. Rename contents
for file in files:
    if os.path.isfile(file):
        rename_content(file)

# 2. Rename files (bottom-up to avoid path invalidation)
dirs_to_rename = set()
for file in files:
    dirname = os.path.dirname(file)
    basename = os.path.basename(file)
    if 'Monolith' in basename or 'monolith' in basename or 'MONOLITH' in basename:
        new_basename = basename.replace('Monolith', 'Megalith').replace('monolith', 'megalith').replace('MONOLITH', 'MEGALITH')
        new_path = os.path.join(dirname, new_basename)
        git_mv(file, new_path)
    
    # Collect dirs to rename
    parts = dirname.split('/')
    current = ""
    for part in parts:
        if part:
            current = os.path.join(current, part)
            if 'Monolith' in part or 'monolith' in part or 'MONOLITH' in part:
                dirs_to_rename.add(current.replace('\\', '/'))

# 3. Rename directories (sort by length descending to rename deepest first)
for d in sorted(list(dirs_to_rename), key=len, reverse=True):
    parent = os.path.dirname(d)
    base = os.path.basename(d)
    new_base = base.replace('Monolith', 'Megalith').replace('monolith', 'megalith').replace('MONOLITH', 'MEGALITH')
    new_d = os.path.join(parent, new_base).replace('\\', '/')
    git_mv(d, new_d)

print("Renaming complete.")
