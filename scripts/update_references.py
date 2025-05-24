#!/usr/bin/env python3

"""
Chronovyan Reference Updater

This script automatically updates references to old Chronovyan file extensions
in documentation files, replacing them with the new extensions.

Usage:
    python update_references.py [--dry-run]

Options:
    --dry-run  Show changes without actually making them

The script will modify Markdown files in the docs/ directory, updating references
to use the new file extensions.
"""

import os
import re
import sys
from pathlib import Path
from colorama import init, Fore, Style

# Initialize colorama for colored output
init()

# Try to find the project root
def find_project_root():
    current_dir = Path(os.getcwd())
    
    # Walk up the directory tree looking for a directory that has docs/ and src/
    while current_dir != current_dir.parent:
        if (current_dir / "docs").exists() and (current_dir / "src").exists():
            return current_dir
        current_dir = current_dir.parent
    
    # If we couldn't find it, fall back to the current directory
    return Path(os.getcwd())

ROOT_DIR = find_project_root()
DOCS_DIR = ROOT_DIR / "docs"

# Mapping of old extensions to new ones
OLD_TO_NEW_EXTENSIONS = {
    '.chronovyan': '.cvy',
    '.weave': '.cvy',
    '.chron': '.cvy',
    '.tchron': '.tcvy',
    '.qchron': '.qcvy',
    '.anchor': '.ancvy',
    '.flux': '.flxcvy',
    '.shard': '.shdcvy',
    '.infuse': '.infcvy',
    '.tflow': '.tcvy',
    '.chronfig': '.cvyconfig',
    '.tconfig': '.tcvyconfig',
    '.rconfig': '.rcvyconfig',
    '.chronmd': '.cvymd',
    '.tmd': '.tcvymd',
    '.qmd': '.qcvymd'
}

def find_markdown_files(directory):
    """Find all Markdown files recursively in the given directory."""
    return list(directory.glob('**/*.md'))

def update_file_references(file_path, dry_run=False):
    """Update references to old file extensions in a file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        try:
            with open(file_path, 'r', encoding='latin-1') as f:
                content = f.read()
        except:
            print(f"{Fore.RED}Error reading file: {file_path}{Style.RESET_ALL}")
            return 0
    
    original_content = content
    changes = 0
    
    # Update references in different contexts:
    
    # 1. Update references in Markdown links: [text](file.old_ext)
    for old_ext, new_ext in OLD_TO_NEW_EXTENSIONS.items():
        # Look for the pattern [text](anything.old_ext) or [text](anything.old_ext#fragment)
        pattern = r'(\[[^\]]+\]\([^)]*?)(' + re.escape(old_ext) + r')(#[^)]*?)?\)'
        replacement = r'\1' + new_ext + r'\3)'
        new_content = re.sub(pattern, replacement, content)
        if new_content != content:
            changes += len(re.findall(pattern, content))
            content = new_content
    
    # 2. Update references in inline code: `path/to/file.old_ext`
    for old_ext, new_ext in OLD_TO_NEW_EXTENSIONS.items():
        pattern = r'`([^`]*?)' + re.escape(old_ext) + r'`'
        new_content = re.sub(pattern, r'`\1' + new_ext + r'`', content)
        if new_content != content:
            changes += len(re.findall(pattern, content))
            content = new_content
    
    # 3. Update references in code blocks
    for old_ext, new_ext in OLD_TO_NEW_EXTENSIONS.items():
        # Look for the pattern file.old_ext within code blocks
        pattern = r'(```[^\n]*\n[\s\S]*?)' + re.escape(old_ext) + r'([\s\S]*?```)'
        
        def replace_in_codeblock(match):
            codeblock_start = match.group(1)
            codeblock_end = match.group(2)
            # Only replace within the code block, not in the language identifier
            lines = codeblock_start.split('\n')
            first_line = lines[0]  # This has the ```language part
            rest = '\n'.join(lines[1:])
            # Replace the extension in the rest of the block
            rest = rest.replace(old_ext, new_ext)
            return first_line + '\n' + rest + codeblock_end
        
        new_content = re.sub(pattern, replace_in_codeblock, content)
        if new_content != content:
            # Count changes approximately
            changes += content.count(old_ext) - new_content.count(old_ext)
            content = new_content
    
    # Write back the changes
    if content != original_content:
        if not dry_run:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"{Fore.GREEN}Updated {changes} references in {file_path.relative_to(ROOT_DIR)}{Style.RESET_ALL}")
            except Exception as e:
                print(f"{Fore.RED}Error writing to {file_path}: {e}{Style.RESET_ALL}")
        else:
            print(f"{Fore.YELLOW}Would update {changes} references in {file_path.relative_to(ROOT_DIR)}{Style.RESET_ALL}")
    
    return changes

def main():
    """Main function to update all Markdown files."""
    print(f"{Fore.CYAN}=== The Chronovyan Reference Updater ==={Style.RESET_ALL}")
    print(f"{Fore.CYAN}Weaving new temporal threads into the Codex...{Style.RESET_ALL}\n")
    
    # Check for command line arguments
    dry_run = "--dry-run" in sys.argv
    if dry_run:
        print(f"{Fore.YELLOW}Running in dry-run mode. No changes will be made.{Style.RESET_ALL}\n")
    
    if not DOCS_DIR.exists():
        print(f"{Fore.RED}Error: Documentation directory not found at {DOCS_DIR}{Style.RESET_ALL}")
        print(f"{Fore.YELLOW}Make sure you're running this script from the Chronovyan project root or a subdirectory.{Style.RESET_ALL}")
        return 1
    
    markdown_files = find_markdown_files(DOCS_DIR)
    print(f"Found {len(markdown_files)} documentation files to process.\n")
    
    total_changes = 0
    
    for file_path in markdown_files:
        changes = update_file_references(file_path, dry_run)
        total_changes += changes
    
    print("\n" + "=" * 50)
    if total_changes > 0:
        if dry_run:
            print(f"{Fore.YELLOW}Would update {total_changes} references in the documentation.{Style.RESET_ALL}")
            print(f"{Fore.YELLOW}Run without --dry-run to apply these changes.{Style.RESET_ALL}")
        else:
            print(f"{Fore.GREEN}Updated {total_changes} references in the documentation.{Style.RESET_ALL}")
            print(f"{Fore.CYAN}The Chronovyan Codex has been synchronized with the new file extensions.{Style.RESET_ALL}")
    else:
        print(f"{Fore.GREEN}No references needed updating. The Codex is already synchronized.{Style.RESET_ALL}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main()) 