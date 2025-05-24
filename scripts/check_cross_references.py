#!/usr/bin/env python3

"""
Chronovyan Cross-Reference Validator

This script scans the Chronovyan documentation files to find and validate 
cross-references, ensuring that all referenced files exist and that the
link paths are correct for the current directory structure.

Usage:
    python check_cross_references.py

The script will output a report of all cross-references, highlighting those
that appear to be broken or incorrect.
"""

import os
import re
import sys
import urllib.parse
from pathlib import Path

def find_project_root():
    """Find the project root directory (where the script is located)."""
    current_path = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(current_path)  # Go up one level from scripts/
    print(f"Using project root: {project_root}")
    return project_root

def scan_md_files(docs_dir):
    """Scan all markdown files in the docs directory."""
    print(f"Looking for docs in: {docs_dir}")
    md_files = []
    for root, _, files in os.walk(docs_dir):
        for file in files:
            if file.endswith('.md'):
                md_files.append(os.path.join(root, file))
    return md_files

def extract_references(file_path):
    """Extract all references from a markdown file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except UnicodeDecodeError:
        # Try with a different encoding
        try:
            with open(file_path, 'r', encoding='latin-1') as f:
                content = f.read()
        except Exception as e:
            print(f"Warning: Unable to read file {file_path}: {e}")
            return []
    except Exception as e:
        print(f"Warning: Unable to read file {file_path}: {e}")
        return []
    
    # Remove code blocks to avoid treating code as references
    # This regex matches both ```code``` and `inline code` blocks
    no_code_content = re.sub(r'```.*?```', '', content, flags=re.DOTALL)
    no_code_content = re.sub(r'`[^`]+?`', '', no_code_content)
    
    # Extract links using regex
    # Match both [text](link) format and `code-style` references
    links = re.findall(r'\[.+?\]\((.+?)\)', no_code_content)
    code_refs = re.findall(r'`([^`]+?\.(cvy|py|sh|md|cpp|h))`', no_code_content)
    
    # Extract only the path part from code_refs
    code_paths = [ref[0] for ref in code_refs]
    
    return links + code_paths

def validate_reference(ref, project_root, file_path):
    """Validate if a reference points to an existing file."""
    # Skip external URLs
    if ref.startswith(('http://', 'https://')):
        return True
    
    # Skip anchor references
    if ref.startswith('#'):
        return True
    
    # URL decode the reference
    try:
        decoded_ref = urllib.parse.unquote(ref)
    except Exception:
        decoded_ref = ref
    
    # Try with both original and decoded reference
    for current_ref in [ref, decoded_ref]:
        # Handle relative paths
        if current_ref.startswith('../'):
            # Relative to the file's directory, going up
            base_dir = os.path.dirname(file_path)
            target_path = os.path.normpath(os.path.join(base_dir, current_ref))
        elif current_ref.startswith('./'):
            # Relative to the file's directory
            base_dir = os.path.dirname(file_path)
            target_path = os.path.normpath(os.path.join(base_dir, current_ref[2:]))
        else:
            # Either absolute from project root or relative to file location
            if current_ref.startswith('/'):
                # Absolute from project root (remove leading slash)
                target_path = os.path.join(project_root, current_ref[1:])
            else:
                # Try both possibilities
                # 1. First try relative to the file's directory
                file_dir = os.path.dirname(file_path)
                file_relative_path = os.path.join(file_dir, current_ref)
                
                # 2. If not found, try relative to project root
                project_relative_path = os.path.join(project_root, current_ref)
                
                # Check both locations
                if os.path.exists(file_relative_path):
                    return True
                elif os.path.exists(project_relative_path):
                    return True
                else:
                    # Special handling for script references
                    if current_ref.startswith('scripts/'):
                        # Also check from project root
                        direct_path = os.path.join(project_root, current_ref)
                        if os.path.exists(direct_path):
                            return True
                    # Don't return False here, try the other reference format
                    continue
        
        # Check if the path exists
        if os.path.exists(target_path):
            return True
    
    return False

def check_references(md_files, project_root):
    """Check all references in the given markdown files."""
    issues_found = 0
    
    for file_path in md_files:
        try:
            refs = extract_references(file_path)
            file_issues = []
            
            for ref in refs:
                if not validate_reference(ref, project_root, file_path):
                    file_issues.append(f"  - Broken reference: `{ref}` -> File not found: {ref}")
            
            rel_path = os.path.relpath(file_path, project_root)
            if file_issues:
                print(f"Issues in {rel_path}:")
                for issue in file_issues:
                    print(issue)
                issues_found += len(file_issues)
                print()
            else:
                print(f"✓ {rel_path} - All references intact")
        except Exception as e:
            print(f"Error processing file {file_path}: {e}")
            issues_found += 1
    
    return issues_found

def main():
    print("=== The Chronovyan Cross-Reference Validator ===")
    print("Scanning the Archive for broken temporal threads...\n")
    
    project_root = find_project_root()
    docs_dir = os.path.join(project_root, "docs")
    
    md_files = scan_md_files(docs_dir)
    print(f"Found {len(md_files)} documentation files to check.\n")
    
    issues_count = check_references(md_files, project_root)
    
    print("=" * 50)
    print(f"Found {issues_count} issues in the documentation.")
    
    if issues_count > 0:
        print("Please fix these broken references to maintain the integrity of the Chronovyan Codex.")
        print("Many issues may be due to the recent extension change from .chronovyan to .cvy.")
        sys.exit(1)
    else:
        print("All references are intact! The Chronovyan Codex is in perfect harmony.")
        sys.exit(0)

if __name__ == "__main__":
    main() 