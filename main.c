int png_read(const char* filename);
int ico_read(const char* filename);
int ico_write(const char* filename, const char* png_filename);

int main() {
	ico_write("calculator.ico", "calculator.png");
}
