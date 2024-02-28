# Simple doubly linked list for iterating through 

class Node():

	def __init__(self, val=0):
		self.next = None
		self.prev = None
		self.val = val

#DoublyLinkedList

class DLL():

	def __init__(self):
		self.head = None
		self.tail = None

	def __str__(self):
		str_out = ""
		curr = self.head

		while curr != None:
			str_out += f"{curr.val},"
			curr = curr.next

		return str_out

	def append(self, val):
		new_node = Node(val)

		if self.head == None:
			self.head = new_node
			self.tail = new_node
		else:
			new_node.prev = self.tail
			self.tail.next = new_node
			self.tail = new_node

	#remove all nodes up to and including the node passed in
	def remove_up_to(self, node):
		if node.next == None and node.prev == None:
			self.head = None
			self.tail = None
		elif node.next == None:
			self.head = None
			self.tail = None
		else:
			node.next.prev = None
			self.head = node.next




if __name__ == "__main__":
	dll = DLL()
	n1 = Node(1)
	n2 = Node(2)
	n3 = Node(3)

	dll.append(1)
	dll.append(2)
	dll.append(3)
	dll.append(4)
	dll.append(5)


	# iterate through linked list externally
	curr_node = dll.head

	while curr_node != None:
		print(dll)
		if curr_node.val == 3:
			dll.remove_up_to(curr_node)
			curr_node = dll.head
		else:
			curr_node = curr_node.next











