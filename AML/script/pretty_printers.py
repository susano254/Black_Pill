import gdb 

class Matrix33Printer:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		m11 = self.val['m11']
		m12 = self.val['m12']
		m13 = self.val['m13']
		m21 = self.val['m21']
		m22 = self.val['m22']
		m23 = self.val['m23']
		m31 = self.val['m31']
		m32 = self.val['m32']
		m33 = self.val['m33']
		return '[{} {} {}] [{} {} {}] [{} {} {}]'.format(m11, m12, m13, m21, m22, m23, m31, m32, m33)

class Vector3Printer:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		x = self.val['x']
		y = self.val['y']
		z = self.val['z']
		return '[{}, {}, {}]'.format(x, y, z)

class EulerAnglesPrinter:
	def __init__(self, val):
		self.val = val

	def getSequence(self, seq):
		if seq == 0: return 'ZXZ'
		if seq == 1: return 'XYX'
		if seq == 2: return 'YZY'
		if seq == 3: return 'ZYZ'
		if seq == 4: return 'XZX'
		if seq == 5: return 'YXY'
		if seq == 6: return 'XYZ'
		if seq == 7: return 'YZX'
		if seq == 8: return 'ZXY'
		if seq == 9: return 'XZY'
		if seq == 10: return 'ZYX'
		if seq == 11: return 'YZX'

	def to_string(self):
		phi = self.val['phi']
		theta = self.val['theta']
		psi = self.val['psi']
		seq = self.getSequence(self.val['seq'])
		print(self.val['seq'])
		print('[{}, {}, {}] seq: {}'.format(phi, theta, psi, seq))
		return '[{}, {}, {}] seq: {}'.format(phi, theta, psi, seq)

class QuaternionPrinter:
	def __init__(self, val):
		self.val = val

	def to_string(self):
		q0 = self.val['q0']
		q1 = self.val['q1']
		q2 = self.val['q2']
		q3 = self.val['q3']
		return '[{}, {}, {}, {}]'.format(q0, q1, q2, q3)


def my_pretty_printers(val):
	if str(gdb.types.get_basic_type(val.type)) == ('AML::Matrix33'): 
		return Matrix33Printer(val)
	if str(gdb.types.get_basic_type(val.type)) == ('AML::Vector3'): 
		return Vector3Printer(val)
	if str(gdb.types.get_basic_type(val.type)) == ('AML::EulerAngles'): 
		return EulerAnglesPrinter(val)
	if str(gdb.types.get_basic_type(val.type)) == ('AML::Quaternion'): 
		return QuaternionPrinter(val)
	#print(val.type)
	return None

gdb.pretty_printers.append(my_pretty_printers)