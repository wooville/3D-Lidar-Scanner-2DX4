import math
import xlrd
import open3d as o3d
import numpy as np

f = open("points.xyz", "w")

workbook = xlrd.open_workbook("dataset_400192044.xlsx")
sheet = workbook.sheet_by_index(0)
lines = []

# ingesting Excel dataset and processing distance measurement into XYZ format
for x in range(1, 21):
    col = sheet.col_values(x)
    pos = 0
    for dist in col[2:len(col)]:        # measurements start at 3rd element of col
        angle = math.radians((360 / 512) * pos + 270)  # starts facing down, hence +270
        y = dist * (math.sin(angle)) / 1000
        z = dist * (math.cos(angle)) / 1000
        f.write("{} {} {}\n".format(col[1], y, z))  # write converted coordinates to .xyz file
        pos += 1

f.close()

po = 0

# linking every point in plane
for x in range(20):
    for pt in range(512):
        lines.append([pt + po, pt + 1 + po])
    po += 512   # 512 rotations per x

po = 0
do = 512    # index of same point on the next plane will be 512 indices from present point

# linking every point in plane to corresponding point in the next plane
for x in range(19):
    for pt in range(512):
        lines.append([pt + po, pt + do + po])
    po += 512

pcd = o3d.io.read_point_cloud("points.xyz", format='xyz')

line_set = o3d.geometry.LineSet(points=o3d.utility.Vector3dVector(np.asarray(pcd.points)),
                                lines=o3d.utility.Vector2iVector(lines))
o3d.visualization.draw_geometries([line_set])
