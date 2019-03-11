#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cctype>
#include <climits>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;

const uint16_t ENDIAN_LITTLE = 0x4949;
const uint16_t ENDIAN_BIG = 0x4d4d;

#endif /* __PROGTEST__ */

bool to_binary(uint16_t n, uint8_t *binary) {
    if (n > 255) return false;
    else {
        int number = 128;
        for (int i = 0; i < 8; ++i) {
            if (number <= n) {
                n -= number;
                binary[i] = 1;
            } else {
                binary[i] = 0;
            }
            number /= 2;
        }
        return true;
    }
}

uint8_t get_channel(uint8_t a, uint8_t b) {
    if (a == 0 && b == 0) return 1;
    else if (a == 1 && b == 0) return 3;
    else if (a == 1 && b == 1) return 4;
    else return 0;
}

uint8_t get_bits_per_channel(uint8_t a, uint8_t b, uint8_t c) {
    if (a == 0 && b == 0 && c == 0) return 1;
    else if (a == 0 && b == 1 && c == 1) return 8;
    else if (a == 1 && b == 0 && c == 0) return 16;
    else return 0;
}

uint8_t to_number(uint8_t * binary) {
    int number = 128;
    int n = 0;
    for (int i = 0; i < 8; ++i) {
        if (binary[i] == 1) n += number;
        number /= 2;
    }
    return n;
}

class Pixel {
public:
    uint16_t channels[4] = {0};
};

class Img {
public:
    vector <vector<Pixel>> pixels;

    void print() {
        for (auto &i : pixels) {
            for (auto &j : i)
                cout << j.channels[0] << " ";
            cout << endl;
        }
    }
};


bool flipImage(const char *srcFileName, const char *dstFileName, bool flipHorizontal, bool flipVertical) {

    ifstream input_file(srcFileName, ios::binary | ios::in);
    ofstream output_file(dstFileName, ios::binary | ios::in);
    output_file.open(dstFileName);
    if (input_file.fail() || input_file.eof() || !output_file.is_open()) return false;

    uint16_t width, height, format_2B;
    uint16_t read_index = 0;
    bool little_endian;
    char c, d;
    // 8 byte header array
    uint8_t header[8];
    // 8 bits format information array
    uint8_t format_bits[8];
    // for 1 bit per channel
    uint8_t binary[8];
    uint8_t binary2[8];
    uint8_t new_binary[8];
    uint16_t zero_padding = 0;

    // first get header
    while (read_index < 8 && input_file.get(c)) {
        header[read_index] = (uint8_t)(unsigned char)
        c;
        ++read_index;
    }
    read_index = 0;

    // decide what endian we are working with
    // extract width, height and format from header depending on endian
    if ((header[1] << 8 | header[0]) == ENDIAN_LITTLE) {
        little_endian = true;
        width = uint16_t(header[3] << 8 | header[2]);
        height = uint16_t(header[5] << 8 | header[4]);
        format_2B = uint16_t(header[7] << 8 | header[6]);
    } else if ((header[1] << 8 | header[0]) == ENDIAN_BIG) {
        little_endian = false;
        width = uint16_t(header[2] << 8 | header[3]);
        height = uint16_t(header[4] << 8 | header[5]);
        format_2B = uint16_t(header[6] << 8 | header[7]);
    } else return false;

    // convert format_2B information to bits
    to_binary(format_2B, format_bits);

    uint8_t channels = get_channel(format_bits[6], format_bits[7]);
    uint8_t bits_per_channel = get_bits_per_channel(format_bits[3], format_bits[4], format_bits[5]);

    if (channels == 0 || bits_per_channel == 0 || width < 1 || height < 1)
        return false;

    Img src_img = Img();

    cout << endl;
    cout << "width: " << width << endl;
    cout << "height: " << height << endl;
    cout << "channels: " << (int)channels << endl;
    cout << "bpc: " << (int)bits_per_channel << endl;
    int calls = 0;
    // read data and put them in Img class
    if (bits_per_channel == 8) {
        for (int i = 0; i < height; ++i) {
            vector <Pixel> pixels;
            for (int j = 0; j < width; ++j) {
                Pixel pixel = Pixel();
                for (int k = 0; k < channels; ++k) {
                    input_file.get(c);
                    if (input_file.eof()) return false;
                    pixel.channels[k] = (uint16_t)(unsigned char)
                    c;
                }
                pixels.push_back(pixel);
            }
            src_img.pixels.push_back(pixels);
        }
    } else if (bits_per_channel == 16) {
        for (int i = 0; i < height; ++i) {
            vector <Pixel> pixels;
            for (int j = 0; j < width; ++j) {
                Pixel pixel = Pixel();
                for (int k = 0; k < channels; ++k) {
                    input_file.get(c);
                    if (input_file.eof()) return false;
                    auto first_byte = (uint8_t)(unsigned char)c;
                    input_file.get(d);
                    if (input_file.eof()) return false;
                    auto second_byte = (uint8_t)(unsigned char)d;
                    if (little_endian) {
                        pixel.channels[k] = (uint16_t)(second_byte << 8 | first_byte);
                    } else {
                        pixel.channels[k] = (uint16_t)(first_byte << 8 | second_byte);
                    }

                }
                pixels.push_back(pixel);
            }
            src_img.pixels.push_back(pixels);
        }
    } else if (bits_per_channel == 1) {
        if (!little_endian) {
            for (int i = 0; i < height; ++i) {
                vector <Pixel> pixels;
                int j = 0;
                int tmp_zp = 0;
                while (j < width * channels) {
                    if (j == 0) {
                        input_file.get(c);
                        //calls++;
                        if (input_file.eof()) return false;
                        to_binary((uint8_t)(unsigned char) c, binary);
                    }
                    Pixel pixel = Pixel();
                    for (int k = 0; k < channels; ++k) {
                        if (j % 8 == 0 && j != 0) {
                            if (j + 8 > width * channels) {
                                zero_padding = (j + 8) - width * channels;
                                tmp_zp = zero_padding;
                                //cout << j << ":" << zero_padding << endl;
                            }
                            input_file.get(c);
                            //calls++;
                            if (input_file.eof()) return false;
                            to_binary((uint16_t)(unsigned char) c, binary);
                        }
                        //cout << j << "-" << tmp_zp << endl;
                        pixel.channels[k] = binary[(j % 8) + tmp_zp];
                        ++j;
                    }
                    pixels.push_back(pixel);
                }
                //cout << calls << endl;
                src_img.pixels.push_back(pixels);
            }
        } else {
            for (int i = 0; i < height; ++i) {
                vector <Pixel> pixels;
                int j = 0;
                int tmp_zp = 0;
                while (j < width * channels) {
                    if (j == 0) {
                        input_file.get(c);
                        if (input_file.eof()) return false;
                        to_binary((uint8_t)(unsigned char) d, binary2);
                    }
                    Pixel pixel = Pixel();
                    for (int k = 0; k < channels; ++k) {
                        if (j % 8 == 0 && j != 0) {
                            if (j + 8 > width * channels) {
                                zero_padding = (j + 8) - width * channels;
                                tmp_zp = zero_padding;
                                //cout << j << ":" << zero_padding << endl;
                            }
                            input_file.get(c);
                            //calls++;
                            if (input_file.eof()) return false;
                            to_binary((uint16_t)(unsigned char) c, binary);
                        }
                        //cout << j << "-" << tmp_zp << endl;
                        pixel.channels[k] = binary[(j % 8) + tmp_zp];
                        ++j;
                    }
                    pixels.push_back(pixel);
                }
                //cout << calls << endl;
                src_img.pixels.push_back(pixels);
            }
        }
    }
    input_file.get(c);
    if (!input_file.eof()) return false;

    src_img.print();
    // horizontal and vertical flip
    if (flipHorizontal) {
        for (auto &row : src_img.pixels) {
            reverse(row.begin(), row.end());
        }
    }
    if (flipVertical) {
        reverse(src_img.pixels.begin(), src_img.pixels.end());
    }
    cout << endl;
    src_img.print();

    // insert header into output file
    for (auto i : header) {
        output_file.put((char) i);
        if (output_file.bad()) return false;
    }

    // insert rest of the data into output file
    for (int i = 0; i < height; ++i) {
        uint8_t binary_index = 0;
        for (int j = 0; j < width; ++j) {
            for (int k = 0; k < channels; ++k) {
                if (bits_per_channel == 1) {
                    if (binary_index % 8 == 0 && binary_index != 0) {
                        output_file.put((char) to_number(binary));
                    }
                    binary[binary_index % 8] = src_img.pixels[i][j].channels[k];
                    ++binary_index;
                    if (binary_index == width * channels) {
                        for (int l = 0; l < 8; ++l) {
                            if (l < zero_padding)
                                new_binary[l] = 0;
                            else
                                new_binary[l] = binary[l - zero_padding];
                        }
                        output_file.put((char) to_number(new_binary));
                    }
                } else if (bits_per_channel == 16) {
                    if (little_endian) {
                        output_file.put((char) src_img.pixels[i][j].channels[k]);
                        uint8_t byte2 = src_img.pixels[i][j].channels[k] >> 8;
                        output_file.put((char) byte2);
                    } else {
                        uint8_t byte2 = src_img.pixels[i][j].channels[k] >> 8;
                        output_file.put((char) byte2);
                        output_file.put((char) src_img.pixels[i][j].channels[k]);
                    }

                } else {
                    output_file.put((char) src_img.pixels[i][j].channels[k]);
                }
                if (output_file.bad()) return false;
            }
        }
    }
    input_file.close();
    output_file.close();


    return true;
}

#ifndef __PROGTEST__

void showFile(const char *fileName) {
    char c;
    int offset = 0;
    ifstream f(fileName, ios::binary | ios::in);
    while (f.get(c)) {
        cout << setw(3) << (int) (unsigned char) c;
        if (offset == 7)
            cout << endl;
        if (offset % 2 == 1 && offset > 8)
            cout << endl;
        offset++;
    }
}

bool identicalFiles(const char *fileName1, const char *fileName2) {
    cout << endl;
    showFile(fileName1);
    cout << endl;
    showFile(fileName2);
    return true;
}

int main() {
//    flipImage("input_00.img", "output_00.img", true, false);
//    identicalFiles("input_00.img", "ref_00.img");

//    assert(flipImage("input_00.img", "output_00.img", true, false)
//           && identicalFiles("output_00.img", "ref_00.img"));
//
//    assert(flipImage("input_01.img", "output_01.img", false, true)
//           && identicalFiles("output_01.img", "ref_01.img"));
//
//    assert(flipImage("input_02.img", "output_02.img", true, true)
//           && identicalFiles("output_02.img", "ref_02.img"));
//
//    assert(flipImage("input_03.img", "output_03.img", false, false)
//           && identicalFiles("output_03.img", "ref_03.img"));
//
//    assert(flipImage("input_04.img", "output_04.img", true, false)
//           && identicalFiles("output_04.img", "ref_04.img"));
//
//    assert(flipImage("input_05.img", "output_05.img", true, true)
//           && identicalFiles("output_05.img", "ref_05.img"));
//
//    assert(flipImage("input_06.img", "output_06.img", false, true)
//           && identicalFiles("output_06.img", "ref_06.img"));
//
//    assert(flipImage("input_07.img", "output_07.img", true, false)
//           && identicalFiles("output_07.img", "ref_07.img"));
//
//    assert(flipImage("input_08.img", "output_08.img", true, true)
//           && identicalFiles("output_08.img", "ref_08.img"));
//
//    assert(!flipImage("input_09.img", "output_09.img", true, false));
//
//     extra inputs (optional & bonus tests)
//    assert(flipImage("extra_input_00.img", "extra_out_00.img", true, false)
//           && identicalFiles("extra_out_00.img", "extra_ref_00.img"));
//    assert(flipImage("extra_input_01.img", "extra_out_01.img", false, true)
//           && identicalFiles("extra_out_01.img", "extra_ref_01.img"));
//    assert(flipImage("extra_input_02.img", "extra_out_02.img", true, false)
//           && identicalFiles("extra_out_02.img", "extra_ref_02.img"));
//    assert(flipImage("extra_input_03.img", "extra_out_03.img", false, true)
//           && identicalFiles("extra_out_03.img", "extra_ref_03.img"));
//    assert(flipImage("extra_input_04.img", "extra_out_04.img", true, false)
//           && identicalFiles("extra_out_04.img", "extra_ref_04.img"));
//    assert(flipImage("extra_input_05.img", "extra_out_05.img", false, true)
//           && identicalFiles("extra_out_05.img", "extra_ref_05.img"));
//    assert(flipImage("extra_input_06.img", "extra_out_06.img", true, false)
//           && identicalFiles("extra_out_06.img", "extra_ref_06.img"));
//    assert(flipImage("extra_input_07.img", "extra_out_07.img", false, true)
//           && identicalFiles("extra_out_07.img", "extra_ref_07.img"));
    identicalFiles("extra_input_08.img", "extra_ref_08.img");
    assert(flipImage("extra_input_08.img", "extra_out_08.img", true, false)
           && identicalFiles("extra_out_08.img", "extra_ref_08.img"));
//    assert(flipImage("extra_input_09.img", "extra_out_09.img", false, true)
//           && identicalFiles("extra_out_09.img", "extra_ref_09.img"));
//    identicalFiles("extra_input_10.img", "extra_ref_10.img");
//    assert(flipImage("extra_input_10.img", "extra_out_10.img", true, false)
//           && identicalFiles("extra_out_10.img", "extra_ref_10.img"));
//
//    identicalFiles("extra_input_11.img", "extra_ref_11.img");
//    assert(flipImage("extra_input_11.img", "extra_out_11.img", false, true)
//           && identicalFiles("extra_out_11.img", "extra_ref_11.img"));
    return 0;
}

#endif /* __PROGTEST__ */
