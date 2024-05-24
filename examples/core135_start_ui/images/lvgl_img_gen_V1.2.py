import os
import copy
import os
import shutil
def rename_files_in_directory(directory_path):
    # 遍历目录中的所有文件和文件夹
    for filename in os.listdir(directory_path):
        # 检查文件名中是否包含空格
        text_f = [' ', '-', '"', '“', '”', '(', ')', '“', '”', ',']
        newfile = filename[:]
        for t in text_f:
            if t in filename:
                newfile = newfile.replace(t, '_')
            # print(newfile)
        # print(filename)
        if newfile != filename:
            original_path = os.path.join(directory_path, filename)
            new_path = os.path.join(directory_path, newfile)
            os.rename(original_path, new_path)
            print(f'Renamed "{filename}" to "{newfile}"')





import math
import os
import os.path
from PIL import Image

class Converter:

    CF_RAW = 12
    CF_TRUE_COLOR_ALPHA = 101

    CF_TRUE_COLOR_332 = 0
    CF_TRUE_COLOR_565 = 1

    CF_TRUE_COLOR_565_SWAP = 2

    CF_TRUE_COLOR_888 = 3

    def __init__(self, path, real_name, out_name, dith, cf, folders):
        Gif_fn = out_name.split("/")
        out_filename = Gif_fn[len(Gif_fn) - 1]
        self.dith = dith
        self.out_filename = out_filename
        self.out_filepath = folders[1] + real_name
        self.cf = cf
        self.real_name = real_name
        self.out_name = out_name
        self.path = path
        self.im = Image.open(path)
        gif_image = self.im.size
        self.w = gif_image[0]
        self.h = gif_image[1]

        self.r_act = 0
        self.g_act = 0
        self.b_act = 0
        self.dith = None  # Dithering enable/disable
        self.cf = None  # Color format
        self.alpha = None  # Add alpha byte or not
        self.chroma = None  # Chroma keyed?
        self.d_out = None  # Output data (result)
        self.out_name = os.path.basename(path).split(".")[0]  # Name of the output file

        # Helper variables
        self.r_act = 0
        self.b_act = 0
        self.g_act = 0

        # For dithering
        self.r_earr = None  # Classification error for next row of pixels
        self.g_earr = None
        self.b_earr = None

        self.r_nerr = None  # Classification error for next pixel
        self.g_err = None
        self.b_nerr = None

        if cf == "raw" or cf == "raw_alpha" or cf == "raw_chroma":
            return
        self.img: Image.Image = Image.open(path)
        self.w, self.h = self.im.size

        if self.dith:
            self.r_earr = [0] * (self.w + 2)
            self.g_earr = [0] * (self.w + 2)
            self.b_earr = [0] * (self.w + 2)

        self.r_nerr = 0
        self.g_nerr = 0
        self.b_nerr = 0

    def convert(self, cf, alpha=0):
        self.cf = cf
        # print(self.cf)
        # self.d_out = OrderedDict([])
        self.d_out = list()
        self.alpha = alpha

        if self.cf == self.CF_RAW:

            myfile = open(self.path, "rb")
            file_size = os.path.getsize(self.path)
            self.d_out = dict(enumerate(myfile.read(file_size)))

            myfile.close()
        else:
            y = 0
            while (y < self.h):
                self.dith_reset()
                x = 0
                while (x < self.w):
                    self.conv_px(x, y)
                    x += 1

                y += 1

    def format_to_c_array(self):
        c_array = ""
        i = 0
        y_end = self.h
        x_end = self.w

        if (self.cf == self.CF_TRUE_COLOR_332):
            c_array += "\n#if LV_COLOR_DEPTH == 1 || LV_COLOR_DEPTH == 8"
            if (not self.alpha):
                c_array += "\n  /*Pixel format: Blue: 2 bit, Green: 3 bit, Red: 3 bit*/"
            else:
                c_array += "\n  /*Pixel format: Blue: 2 bit, Green: 3 bit, Red: 3 bit, Alpha 8 bit */"

        elif (self.cf == self.CF_TRUE_COLOR_565):

            c_array += "\n#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0"
            if (not self.alpha):
                c_array += "\n  /*Pixel format: Blue: 5 bit, Green: 6 bit, Red: 5 bit*/"
            else:
                c_array += "\n  /*Pixel format: Blue: 5 bit, Green: 6 bit, Red: 5 bit, Alpha 8 bit*/"

        elif (self.cf == self.CF_TRUE_COLOR_565_SWAP):
            c_array += "\n#if LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0"
            if (not self.alpha):
                c_array += "\n  /*Pixel format: Blue: 5 bit, Green: 6 bit, Red: 5 bit BUT the 2 bytes are swapped*/"
            else:
                c_array += "\n  /*Pixel format:  Blue: 5 bit Green: 6 bit, Red: 5 bit, Alpha 8 bit  BUT the 2  color bytes are swapped*/"

        elif (self.cf == self.CF_TRUE_COLOR_888):

            c_array += "\n#if LV_COLOR_DEPTH == 32"
            if (not self.alpha):
                c_array += "\n  /*Pixel format: Blue: 8 bit, Green: 8 bit, Red: 8 bit, Fix 0xFF: 8 bit, */"
            else:
                c_array += "\n  /*Pixel format:  Blue: 8 bit, Green: 8 bit, Red: 8 bit, Alpha: 8 bit*/"

        elif (self.cf == self.CF_RAW):
            y_end = 1
            x_end = len(self.d_out)
            i = 0

        y = 0
        while (y < y_end):
            c_array += "\n  "
            x = 0
            while (x < x_end):

                if (self.cf == self.CF_TRUE_COLOR_332):


                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1
                    if (self.alpha):
                        if len(str(hex(self.d_out[i]))[2:]) < 2:
                            c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "

                        else:
                            c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                        i += 1
                elif self.cf == self.CF_TRUE_COLOR_565 or self.cf == self.CF_TRUE_COLOR_565_SWAP:
                    # print("ASfasfasdgdasfdafadsf", self.d_out)
                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1

                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1

                    if (self.alpha):

                        if len(str(hex(self.d_out[i]))[2:]) < 2:
                            c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                        else:
                            c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                        i += 1

                elif (self.cf == self.CF_TRUE_COLOR_888):
                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1

                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1

                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1

                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "
                    i += 1


                elif (self.cf == self.CF_RAW):

                    if len(str(hex(self.d_out[i]))[2:]) < 2:
                        c_array += '0x' + '0' + str(hex(self.d_out[i]))[2:] + ", "
                    else:
                        c_array += '0x' + str(hex(self.d_out[i]))[2:] + ", "

                    if (i != 0 and i % 16 == 0):
                        c_array += "\n  "

                    i += 1
                x += 1
            y += 1

        if (self.cf == self.CF_TRUE_COLOR_332 or self.cf == self.CF_TRUE_COLOR_565 or self.cf == self.CF_TRUE_COLOR_565_SWAP or self.cf == self.CF_TRUE_COLOR_888):
            c_array += "\n#endif"

        return c_array
    def get_c_header(self):
        c_header = "#include \"../ui.h\"\n\n\n#ifndef LV_ATTRIBUTE_MEM_ALIGN\n#define LV_ATTRIBUTE_MEM_ALIGN\n#endif\n\n"
        attr_name = "LV_ATTRIBUTE_IMG_" + self.out_filename.upper()
        c_header += str(str("#ifndef " + str(attr_name) + "\n#define " + str(
            attr_name) + "\n#endif\n\nconst LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST " + str(
            attr_name) + " uint8_t ") + str(self.out_filename)) + "_map[] = {"
        return c_header

    def get_c_footer(self, cf):
        c_footer = str(str(str(str(str("\n};\n\nconst lv_img_dsc_t " + str(
            self.out_filename)) + " = {\n  .header.always_zero = 0,\n  .header.w = ") + str(
            self.w)) + ",\n  .header.h = ") + str(self.h)) + ",\n"

        if cf == self.CF_RAW:
            c_footer += str("  .data_size = " + str(
                len(self.d_out))) + ",\n  .header.cf = LV_IMG_CF_RAW,"
        elif cf == self.CF_TRUE_COLOR_ALPHA:
            c_footer += str("  .data_size = " + str(
                self.w * self.h)) + " * LV_IMG_PX_SIZE_ALPHA_BYTE,\n  .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,"

        c_footer += str("\n  .data = " + str(self.out_filename)) + "_map,\n};\n"
        return c_footer

    def conv_px(self, x, y):
        c = self.im.getpixel((x, y))
        a = c[3] if len(c) == 4 else 0xFF
        r, g, b = c[:3]

        self.dith_next(r, g, b, x)
        if self.cf == self.CF_TRUE_COLOR_332:

            c8 = self.r_act | self.g_act >> 3 | self.b_act >> 6
            # RGB332
            self.d_out.append(c8)

            if self.alpha:
                self.d_out.append(a)

        else:
            if self.cf == self.CF_TRUE_COLOR_565:
                c16 = self.r_act << 8 | self.g_act << 3 | self.b_act >> 3
                # RGR565
                self.d_out.append(c16 & 0xff)
                self.d_out.append(c16 >> 8 & 0xff)
                if self.alpha:
                    self.d_out.append(a)

            else:
                if self.cf == self.CF_TRUE_COLOR_565_SWAP:
                    c16 = self.r_act << 8 | self.g_act << 3 | self.b_act >> 3
                    # RGR565
                    self.d_out.append(c16 >> 8 & 0xff)
                    self.d_out.append(c16 & 0xff)
                    if self.alpha:
                        self.d_out.append(a)

                else:
                    if self.cf == self.CF_TRUE_COLOR_888:
                        self.d_out.append(self.b_act)
                        self.d_out.append(self.g_act)
                        self.d_out.append(self.r_act)
                        self.d_out.append(a)

    def dith_reset(self):
        if self.dith:
            self.r_nerr = 0
            self.g_nerr = 0
            self.b_nerr = 0

    def dith_next(self, r, g, b, x):

        if self.dith:
            self.r_act = r + self.r_nerr + self.r_earr[x + 1]
            self.r_earr[x + 1] = 0
            self.g_act = g + self.g_nerr + self.g_earr[x + 1]
            self.g_earr[x + 1] = 0
            self.b_act = b + self.b_nerr + self.b_earr[x + 1]
            self.b_earr[x + 1] = 0
            if self.cf == self.CF_TRUE_COLOR_332:
                self.r_act = self.classify_pixel(self.r_act, 3)
                self.g_act = self.classify_pixel(self.g_act, 3)
                self.b_act = self.classify_pixel(self.b_act, 2)
                if self.r_act > 0xe0:
                    self.r_act = 0xe0

                if self.g_act > 0xe0:
                    self.g_act = 0xe0

                if self.b_act > 0xc0:
                    self.b_act = 0xc0

            else:
                if self.cf == self.CF_TRUE_COLOR_565 or self.cf == self.CF_TRUE_COLOR_565_SWAP:
                    self.r_act = self.classify_pixel(self.r_act, 5)
                    self.g_act = self.classify_pixel(self.g_act, 6)
                    self.b_act = self.classify_pixel(self.b_act, 5)
                    if self.r_act > 0xf8:
                        self.r_act = 0xf8

                    if self.g_act > 0xfc:
                        self.g_act = 0xfc

                    if self.b_act > 0xf8:
                        self.b_act = 0xf8

                else:
                    if self.cf == self.CF_TRUE_COLOR_888:
                        self.r_act = self.classify_pixel(self.r_act, 8)
                        self.g_act = self.classify_pixel(self.g_act, 8)
                        self.b_act = self.classify_pixel(self.b_act, 8)
                        if self.r_act > 0xff:
                            self.r_act = 0xff

                        if self.g_act > 0xff:
                            self.g_act = 0xff

                        if self.b_act > 0xff:
                            self.b_act = 0xff

            self.r_err = r - self.r_act
            self.g_err = g - self.g_act
            self.b_err = b - self.b_act
            self.r_nerr = round(7 * self.r_err / 16)
            self.g_nerr = round(7 * self.g_err / 16)
            self.b_nerr = round(7 * self.b_err / 16)
            self.r_earr[x] += round(3 * self.r_err / 16)
            self.g_earr[x] += round(3 * self.g_err / 16)
            self.b_earr[x] += round(3 * self.b_err / 16)
            self.r_earr[x + 1] += round(5 * self.r_err / 16)
            self.g_earr[x + 1] += round(5 * self.g_err / 16)
            self.b_earr[x + 1] += round(5 * self.b_err / 16)
            self.r_earr[x + 2] += round(self.r_err / 16)
            self.g_earr[x + 2] += round(self.g_err / 16)
            self.b_earr[x + 2] += round(self.b_err / 16)
        else:
            if self.cf == self.CF_TRUE_COLOR_332:
                self.r_act = self.classify_pixel(r, 3)
                self.g_act = self.classify_pixel(g, 3)
                self.b_act = self.classify_pixel(b, 2)
                # print(self.r_act)
                if self.r_act > 0xe0:
                    self.r_act = 0xe0

                if self.g_act > 0xe0:
                    self.g_act = 0xe0

                if self.b_act > 0xc0:
                    self.b_act = 0xc0

            else:
                if self.cf == self.CF_TRUE_COLOR_565 or self.cf == self.CF_TRUE_COLOR_565_SWAP:
                    self.r_act = self.classify_pixel(r, 5)
                    self.g_act = self.classify_pixel(g, 6)
                    self.b_act = self.classify_pixel(b, 5)
                    if self.r_act > 0xf8:
                        self.r_act = 0xf8

                    if self.g_act > 0xfc:
                        self.g_act = 0xfc

                    if self.b_act > 0xf8:
                        self.b_act = 0xf8

                else:
                    if self.cf == self.CF_TRUE_COLOR_888:
                        self.r_act = self.classify_pixel(r, 8)
                        self.g_act = self.classify_pixel(g, 8)
                        self.b_act = self.classify_pixel(b, 8)
                        if self.r_act > 0xff:
                            self.r_act = 0xff

                        if self.g_act > 0xff:
                            self.g_act = 0xff

                        if self.b_act > 0xff:
                            self.b_act = 0xff

    def classify_pixel(self, value, bits):
        tmp = 1 << (8 - bits)
        val = math.ceil(value / tmp) * tmp
        return val if val >= 0 else 0

    def download_c(self, name, cf=-1, content=""):
        if len(content) < 1:
            content = self.format_to_c_array()

        if cf < 0:
            cf = self.cf

        out = str(str(str(self.get_c_header()) + str(content)) + "") + str(self.get_c_footer(cf))

        name = str(self.out_filepath) + ".c"
        csp = name.split("/")
        le_csp = len(csp)
        print(csp[le_csp - 1], "Converted successfully")
        file = open(name, "w")
        file.write(out)
        file.close()


import os
import subprocess
import natsort
# from core.png_gif_c_raw import *
from PIL import Image
from numpy import *
import glob
import shutil


def convert_gif(subdir, add_outfd, files_png):
    frames = []
    for file in files_png:
        img = os.path.join(subdir, file)
        new_frame = Image.open(img)
        frames.append(new_frame)
    frames[0].save('{}.gif'.format(add_outfd), format='GIF',
                   append_images=frames[1:],
                   save_all=True,
                   duration=30,
                   loop=0)


def pngs_to_gifs(folders):
    for (subdir, dirs, files) in os.walk("images", topdown=False):
        files_png = natsort.natsorted(files, reverse=False)
        sub_splt = subdir.split(os.sep)
        add_outfd = folders[0] + sub_splt[len(sub_splt) - 1]
        print(sub_splt)
        if len(sub_splt) == 2:
            convert_gif(subdir, add_outfd, files_png)
        else:
            for fp in files_png:
                shutil.copy(folders[2] + fp, folders[0] + fp)


def optimize_gif_file():
    gfile = []
    gif_files = glob.glob(folders[0] + "*.gif")
    if len(gif_files) == 0:
        print("PNG to GIF conversion failed., Give an input PNG in the images folder and follow the folder structure properly!!!")
    else:
        print("Total", len(gif_files), "gifs and cheader")
        for gf in gif_files:
            subprocess.run('convert {} -fuzz 0.1% -layers Optimize {}'.format(gf, gf), shell=True)
            gs = gf.split(os.sep)
            ln = len(gs)
            print(gs[ln - 1], "Converted successfully")
            gfile.append(gf)
    return gfile


def gif_to_cBinary(folders):
    cf = ["raw", "true_color_alpha"]
    dith = 0
    for subdir, dirs, files in os.walk(folders[0]):
        for procesed_file in files:

            check_extension = procesed_file.split(".")

            if check_extension[1] == "gif":
                c_headers_folders = folders[0] + check_extension[0]
                fn = folders[0] + procesed_file
                conv = Converter(fn, check_extension[0], c_headers_folders, dith, cf[0], folders)
                conv.convert(conv.CF_RAW)
                conv.download_c(conv.out_name)

            elif check_extension[1] == "png":

                alpha = 1
                fn = folders[0] + procesed_file
                c_headers_folders = folders[1] + check_extension[0]

                conv = Converter(fn, check_extension[0], c_headers_folders, dith, cf[1], folders)

                conv.convert(conv.CF_TRUE_COLOR_332, alpha)

                c_332 = conv.format_to_c_array()
                conv.convert(conv.CF_TRUE_COLOR_565, alpha)
                c_565 = conv.format_to_c_array()
                conv.convert(conv.CF_TRUE_COLOR_565_SWAP, alpha)
                c_565_swap = conv.format_to_c_array()
                conv.convert(conv.CF_TRUE_COLOR_888, alpha)
                c_888 = conv.format_to_c_array()
                c_res = str(str(str(c_332) + str(c_565)) + str(c_565_swap)) + str(c_888)

                if cf[1] == "true_color_alpha":
                    conv.download_c(conv.out_name, conv.CF_TRUE_COLOR_ALPHA, c_res)



temple_h_str = '''#ifndef LV_IMAGE_HEAD_SET_H
#define LV_IMAGE_HEAD_SET_H
{}
#endif
'''

temple_h1_str = '''LV_IMG_DECLARE({});    // {}\n'''


def write_chead(dir):
    files = os.listdir(dir)
    text_str = ""
    for file in files:
        if file.endswith(".c"):
            text_str += temple_h1_str.format(file[:-2], os.path.join(dir,file))
    with open(os.path.join(dir,'img_set_head.h'), 'w') as f:
        f.write(temple_h_str.format(text_str))


if __name__ == "__main__":

    folders = ['gif_png/', 'Cheaders/', "images/"]

    if os.path.exists(folders[0]):
        shutil.rmtree(folders[0])

    if not (os.path.exists(folders[0]) and os.path.exists(folders[1])):
        for fld in folders:
            os.makedirs(os.path.join(fld), exist_ok=True)




    rename_files_in_directory(folders[2])

    pngs_to_gifs(folders)
    optimize_gif_file()
    gif_to_cBinary(folders)

    write_chead(folders[1])
    print("All Done...!")

    
    