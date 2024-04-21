![AMMIT - Animation Modifier, Metadata, and Import Toolkit](Resources/Ammit-banner.png)

**AMMIT** is a plugin for Unreal Engine 5.3 which allows you to create bite-size bits of animation-processing logic and execute them against animation sequences in bulk. In the current version, AMMIT is built around two concepts: **Operations** and **Flows**. Operations are individual little bits of configurable logic, such as things which smooth out root motion movement, correct busted root bones (looking at you, Mixamo), or generate curve tracks after analyzing the animation. Flows are a series of operations which will be executed in sequence.

Right now, AMMIT is still in extremely early stages; it has exactly two operations. One is "Touch", which works like the UNIX command of the same name and merely marks animations dirty so that they can be force-re-saved. The other is "Curve Generator" and can analyze the animation to generate distance-matching curves and such.

While AMMIT in its present form has very limited functionality in terms of what operations are provided, more will be added over time. In addition, project-specific operations can be created in C++ or as Blueprints (by subclassing the "Ammit Base Modifier" class and implementing the appropriate events).

Flows are not extensible; they're simply an asset type you can create in the editor by picking "AMMIT Flow" from the "Animation" category when you right-click to add a new asset in the editor, and opening a Flow asset will let you configure and run the Flow. When a Flow is executed, the order of operations is: 

* **Initialize all operations.** This will call the "Initialize Operation" event, and any operation which returns `false` is removed from the working set for the rest of the flow.
* **Iterate through all animation sequences:**
  * If a separate target directory is set, duplicate the sequence into the target directory structure and use this new copy as the active sequence. Otherwise, alterations will be made to the original sequence.
  * If 'force root motion' and/or 'force root lock' are set, change those values.
  * Read the root transform for every key in the sequence, storing an "Ammit Bone Point" for each; this has the Key, the Time, the Transform, the DeltaTime since the previous key, and the DeltaTransform since the previous transform. Since this is stored in the state, it only needs to be calculated once (if you're only using the root transforms, anyway).
  * Based on those root transforms, store a best-guess at what the sequence is (`Idle`, `TurnInPlace`, `Start`, `Stop`, `Loop`, `Pivot`, or `JumpOrClimb`) in the state.
  * Iterate through all operations, stage by stage. E.g., all operations have `PreProcessAnimation` called before the first one has `ProcessAnimation` called.
    * **PreProcessAnimation**: This is where, for instance, you can do a first pass through the transforms and determine the furthest distance (for start/stop/pivot), etc. Any which return `false` are excluded from the next two steps. No alterations should be made to the animation here, and all sequence-specific state should be thrown out when entering PreProcessAnimation.
    * **ProcessAnimation**: This is where you actually write tracks or otherwise alter the animation. If an operation calls "Mark Flow Dirty" while in this event, AMMIT will refresh the root bone's Ammit Bone Point collection and any operations below it in the flow will have PreProcessAnimation called again so they can reprocess everything.
    * **PostProcessAnimation**: This is a stage where you can generate anything *after* the animation is fully done processing -- for instance, reading the animation to write the final altered form into a pose search database, or automatically generating montages. Anything which should happen to a sequence after all edits are done.
* When all animation sequences are completed, call **FinalReport** on all operations to let them do any final batch output; writing a report into the logfile, for instance, or writing any sort of combined metadata that was collected over the course of the flow.

If you've got useful operations you'd like to contribute to AMMIT -- or a better UX for configuring the operations -- please feel free to make a pull request!