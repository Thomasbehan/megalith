import re

with open('README.md', 'r', encoding='utf-8') as f:
    content = f.read()

# Replace URLs
content = content.replace('tumourlove/megalith', 'Thomasbehan/megalith')

# Fix the intro
content = content.replace(
    'This is a community-focused fork of Megalith created',
    'This is a community-focused fork of the original Monolith project, renamed to Megalith, created'
)
content = content.replace(
    'upstream Megalith, this fork was established',
    'upstream, this fork (Megalith) was established'
)

# Also check Megalith.uplugin to ensure URLs are correct
with open('Megalith.uplugin', 'r', encoding='utf-8') as f:
    uplugin_content = f.read()
uplugin_content = uplugin_content.replace('tumourlove/megalith', 'Thomasbehan/megalith')
uplugin_content = uplugin_content.replace('"CreatedBy": "tumourlove"', '"CreatedBy": "Thomasbehan and Contributors"')
with open('Megalith.uplugin', 'w', encoding='utf-8') as f:
    f.write(uplugin_content)

with open('README.md', 'w', encoding='utf-8') as f:
    f.write(content)
