---
name: megalith-release
description: Explains the exact process to publish a Megalith plugin release. Use when asked to create a release, make a release, or publish the plugin. Automatically detects the host project's engine version.
---

# Megalith Plugin Release Process

This workflow leverages a hybrid approach: automated GitHub Actions for administrative release generation and local scripts for Unreal Engine C++ compilation.

## Why this workflow?
Compiling an Unreal Engine plugin for Windows requires the engine itself (~50GB+). GitHub's default \windows-latest\ runners cannot host this. Therefore, the GitHub Action automatically scaffolds a Draft release and release notes, while the actual Windows DLL compilation happens locally via \make_release.ps1\.

## Generic Engine Auto-Detection
The \make_release.ps1\ script is fully generic. It reads the \.uproject\ file in the host project's root directory, extracts the \EngineAssociation\ (e.g., 5.4, 5.7), automatically locates the corresponding Unreal Engine installation via the registry, and builds the plugin for that specific engine. 

## Step-by-Step Instructions

1. **Tag and Push (Triggers Automation)**
   Tag the \master\ branch with the desired version and push to GitHub:
   \\\ash
   git tag v0.22.1
   git push origin v0.22.1
   \\\
   *What happens:* The \.github/workflows/release.yml\ action will trigger. It automatically creates a **Draft** GitHub Release and generates release notes based on the commits and PRs since the last tag.

2. **Build Windows Binaries Locally**
   On your local Windows machine, run the build script. By default, it builds for your current project's engine version, and attempts to build for a hardcoded UE 5.8 \FIVEPOINT8\ sibling project. Use the \-SkipUE58\ flag to cleanly bypass the 5.8 sibling build if you do not have it.
   \\\powershell
   .\Scripts\make_release.ps1 -Version "0.22.1" -SkipUE58
   \\\
   *What happens:* 
   - Auto-detects the host project's engine version.
   - Compiles the plugin with optional dependencies disabled.
   - Runs a full-unity collision gate.
   - Packages the binaries into \Megalith-v0.22.1-UE<Version>.zip\ and a legacy copy \Megalith-v0.22.1.zip\.
   - Runs a dumpbin hard-link import smoke test.
   - Prints the exact SHA256 markers to the console.

3. **Upload Artifacts and Add SHA Markers**
   - Go to the GitHub Releases page and edit the Draft release.
   - Upload the generated \.zip\ files (e.g., \Megalith-v0.22.1-UE5.7.zip\ and \Megalith-v0.22.1.zip\).
   - Append the exact SHA256 markers printed by the PowerShell script to the bottom of the release notes. *Do not modify the format of these markers; the auto-updater relies on them.*

4. **Publish the Release**
   - Click "Publish release".
   
## Important Rules
- **Never publish without binaries:** Do not flip the Draft release to Published before uploading the \.zip\ files. Existing deployed auto-updater clients check the "latest" release; if they see a published release with no binaries, they will download GitHub's default source zipball (which lacks compiled DLLs) and break their installation.
- **Do not use \-AllowDirtyTree\ or \-AllowUnverifiedImports\** for production releases. These are strictly for local testing.
