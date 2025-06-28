import matplotlib.pyplot as plt

# Replace these lists with your actual data
execution_times = [1.9579, 1.5639, 0.8240, 0.7549, 0.6719, 0.6520]  # X-axis: Execution Time (s)
thread_counts =   [1, 2, 4, 8, 12, 16]     # Y-axis: Number of Threads

plt.figure(figsize=(10, 6))
plt.plot(thread_counts,execution_times, marker='o', linestyle='-', color='purple')
plt.title("Number of Threads vs Execution Time")
plt.xlabel("Number of Threads")
plt.ylabel("Execution Time (seconds)")
plt.grid(True)
#plt.gca().invert_yaxis()  # Optional: so more threads are at the top
plt.tight_layout()
plt.show()
