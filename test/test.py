import time
from random import randint

def test_complexity():
	"""
	Test the complexity of the code.
	"""

	# Generate a list of random integers
	data = [randint(1, 100) for _ in range(1000)]

	# Measure the time taken to sort the list
	start_time = time.time()
	sorted_data = sorted(data)
	end_time = time.time()

	# Print the time taken
	print(f"Time taken to sort 1000 integers: {end_time - start_time:.6f} seconds")
