import matplotlib.pyplot as plt

# Replace these lists with your actual data
execution_times = [68261.86, 39173.23, 22239.02, 15410.27, 11530.68, 9607.54, 6171.47, 4420.01, 3633.36]  # X-axis: Execution Time (s)
block_size =   [1024, 2048, 4096,  8192, 16384, 32768, 65536, 131072, 262144]     # Y-axis: Number of Threads

plt.figure(figsize=(10, 6))
plt.plot(block_size,execution_times, marker='o', linestyle='-', color='purple')
plt.title("Block size vs Execution Time")
plt.xlabel("Block size")
plt.ylabel("Execution Time (miliseconds)")
plt.grid(True)
#plt.gca().invert_yaxis()  # Optional: so more threads are at the top
plt.tight_layout()
plt.show()
