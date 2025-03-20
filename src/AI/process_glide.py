import time

DATA_FILE = "/home/hardik/GitHub/glide-typing-physical-keyboard/data/glide_input.txt"

def read_key_data():
    """Reads key coordinates from the saved file."""
    try:
        with open(DATA_FILE, "r") as f:
            lines = f.readlines()
        return [tuple(map(int, line.strip().split())) for line in lines]
    except FileNotFoundError:
        print("Error: Key data file not found!")
        return []

def process_glide(keys):
    """Dummy processing function (will replace with AI later)."""
    if not keys:
        return "No input detected"
    
    # Convert key coordinates to dummy letters (for now)
    word = "".join(chr(x) for x, _ in keys)
    
    return word  # Later, replace with real AI prediction

if __name__ == "__main__":
    print("Processing glide input...")
    key_data = read_key_data()
    predicted_word = process_glide(key_data)
    
    print("Predicted word:", predicted_word)
    time.sleep(1)  # Simulate processing delay
