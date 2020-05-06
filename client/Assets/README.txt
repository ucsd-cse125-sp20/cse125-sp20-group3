1. In Blender's Scripting tab, open export_animations.py.

2. Make sure the two booleans at the top are set to true. Run the script (Alt+P).

2b. Sanity Check: Open the generated riggedMesh.gltf (with 3dViewer or such) and ensure model is in unposed orientation

3. Go into the generated animations/ folder and ensure only the desired/valid animations are present. Delete files if necessary

4. Move the entire generated folder into Assets/Animation

5. Run process_animations.bat

5b. Sanity Check: ensure .ozz files are generated for each gltf file