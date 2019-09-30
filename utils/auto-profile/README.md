## Generate Motion Profile from SVG

As a convenience for generating a motion profile for autonomous, you can edit an SVG (through Figma) to specify points. Any circle with opacity 0.98 specifies one point on the path, with points earlier in the SVG file specifying points earlier in time.

In order to use Figma to edit this profile, it is convenient to create a component representing the robot with a circle in the center. Then to specify a new point on the path, duplicate the component and move/rotate as necessary.

For Figma, note that the components should seem to be in REVERSE order. The starting position should be at the bottom (when looking at the sidebar on the left) and the ending position should be at the top. To export, select the frame, then Export as SVG in the bottom right.
