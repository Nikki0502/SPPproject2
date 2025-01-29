# Use an official Ubuntu image as the base
FROM ubuntu:22.04

# Install necessary tools
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    gdb \
    valgrind \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the project files into the container
COPY . .

# Default command (build and test)
CMD ["make", "test"]