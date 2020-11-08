from math import sin, cos, pi, sqrt

SPEED = 1481
HYDROPHONE_DISTANCE = 0.57
SOURCEPOWER = 177 #db


maxTimeDelta = HYDROPHONE_DISTANCE/SPEED
minTimeDelta = 0

def distance(i):
	r = sqrt(SOURCEPOWER/(4*pi*i))
	return r


def roughAngle(timeDelta):
	fraction = (timeDelta)/(maxTimeDelta-minTimeDelta)
	return (pi/2)*fraction

def absoluteAngle(tl, tr):
	closest = min([tl, tr])
	farthest = max([tl, tr])
	timeDelta = farthest-closest
	angle = roughAngle(timeDelta)
	if closest == tr:
		right = True
	else:
		right = False
	return angle, right

def frontBack(tl, tr, tb):
	tf = (max([tl, tr])-min([tl, tr]))/2
	if tf < tb:
		b = True
	else:
		b = False
	return b

def determintePoint(tl, il, tr, ir, tb, ib):
	angle, right = absoluteAngle(tl ,tr)
	front = frontBack(tl, tr, tb)
	dl = distance(il)
	dr = distance(ir)
	db = distance(ib)
	distances = [dl, dr, db]
	POINT_DISTANCE = sum(distances)/len(distances)
	if not front:
		y = POINT_DISTANCE*sin(-angle)
	else:
		y = POINT_DISTANCE*sin(angle)
	if not right:
		x = POINT_DISTANCE*cos(angle) * -1
	else:
		x = POINT_DISTANCE*cos(angle)
	return (x, y)


