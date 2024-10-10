# voxel-engine
not quite minecraft yet

# TODO
- Rendering is quite fast at the moment and we can set render distance to 40+ chunks and keep GPU memory usage within reasonable bounds.
- Currently the GPU allocator is wastes a lot of space per chunk and there needs to be ways to dynamically update the chunk meshes efficiently. We can currently only place blocks but not break them. I think We can implement breaking by a DFS on the chunk voxels
- Structure rendering needs to be fixed, I think we need to do this per chunk and then update them once the chunks are closer to the player


![Screenshot from 2024-10-09 18-51-53](https://github.com/user-attachments/assets/607f2f97-c214-4681-94b6-4582d664987e)
