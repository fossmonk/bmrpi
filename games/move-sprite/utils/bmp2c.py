import struct

def bmp_to_c_structure(bmp_path, output_c_path):
    """
    Converts a 24-bit BMP image to a C structure with width, height, and a pixel buffer.

    Args:
        bmp_path (str): Path to the input 24-bit BMP file.
        output_c_path (str): Path for the output .c file.
    """
    try:
        with open(bmp_path, 'rb') as f:
            # Read the BMP file header (14 bytes)
            bmp_header = f.read(14)

            # Check for 'BM' magic number
            if bmp_header[:2] != b'BM':
                raise ValueError("Not a valid BMP file.")

            # Unpack the BMP header to get the pixel data offset
            pixel_array_offset = struct.unpack('<I', bmp_header[10:14])[0]
            
            # Read the DIB header (40 bytes for BITMAPINFOHEADER)
            f.seek(14)
            dib_header = f.read(40)

            # Unpack the width, height, and bits per pixel from the DIB header
            # The format string 'iiH' will not work, as it's not the right offset.
            # We need to extract the data from the correct byte positions.
            width = struct.unpack('<i', dib_header[4:8])[0]
            height = struct.unpack('<i', dib_header[8:12])[0]
            bits_per_pixel = struct.unpack('<H', dib_header[14:16])[0]

            # Check if it's a 24-bit BMP
            if bits_per_pixel != 24:
                raise ValueError(f"Unsupported BMP format. Only 24-bit BMP images are supported, but found {bits_per_pixel}-bit.")

            # Calculate the padding for each row
            # Rows are padded to a multiple of 4 bytes
            row_size = (width * 3 + 3) & ~3 
            padding = row_size - (width * 3)

            # Move to the start of the pixel data
            f.seek(pixel_array_offset)

            # Read pixel data
            pixel_data = []
            for _ in range(height):
                row_pixels = f.read(width * 3)
                f.read(padding)  # Skip padding
                pixel_data.append(row_pixels)
            
            # BMP stores pixels bottom-up, so reverse the rows
            pixel_data.reverse()

            # Convert BGR to ARGB (uint32_t)
            c_array_data = []
            for row in pixel_data:
                for i in range(0, len(row), 3):
                    b = row[i]
                    g = row[i+1]
                    r = row[i+2]
                    # Create 32-bit ARGB value (A=255 for opaque)
                    argb = (0xFF << 24) | (r << 16) | (g << 8) | b
                    c_array_data.append(argb)

        # Write the C structure and array to the output file
        with open(output_c_path, 'w') as f:
            f.write(f'#include <stdint.h>\n\n')
            f.write(f'const uint32_t image_data[{width * height}] = {{\n')
            
            # Format the array data with 16 elements per line for readability
            for i in range(0, len(c_array_data), 16):
                f.write('    ')
                f.write(', '.join([f'0x{val:08X}' for val in c_array_data[i:i+16]]))
                f.write(',\n')
            
            f.write('};\n\n')
            f.write(f'typedef struct {{\n')
            f.write(f'    uint32_t width;\n')
            f.write(f'    uint32_t height;\n')
            f.write(f'    const uint32_t* buffer;\n')
            f.write(f'}} Image;\n\n')
            f.write(f'const Image my_image = {{\n')
            f.write(f'    .width = {width},\n')
            f.write(f'    .height = {height},\n')
            f.write(f'    .buffer = image_data\n')
            f.write(f'}};\n')
        
        print(f"Successfully converted '{bmp_path}' to '{output_c_path}'.")

    except FileNotFoundError:
        print(f"Error: The file '{bmp_path}' was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == '__main__':
    bmp_to_c_structure('../res/vampire_main.bmp', 'vampire_main.c')
    bmp_to_c_structure('../res/vampire_jump.bmp', 'vampire_jump.c')
    bmp_to_c_structure('../res/vampire_magic.bmp', 'vampire_magi.c')
    bmp_to_c_structure('../res/vampire_running.bmp', 'vampire_runn.c')
    bmp_to_c_structure('../res/vampire_side.bmp', 'vampire_side.c')