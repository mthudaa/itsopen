# Makefile untuk proyek-halo dengan GUI (GTK 4)

# Compiler yang digunakan
CC = gcc
# Opsi/flags saat kompilasi. Kita gunakan `pkg-config` untuk mendapatkan flags GTK.
CFLAGS = -Wall $(shell pkg-config --cflags gtk4)

# Flags untuk linker. Kita gunakan `pkg-config` untuk mendapatkan library GTK.
LDFLAGS = $(shell pkg-config --libs gtk4)

# Nama file executable yang akan dihasilkan
TARGET = itsopen

# Direktori source code
SRCDIR = src
# Semua file source .c di dalam SRCDIR
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Target default
all: $(TARGET)

# Resep untuk membuat target, sekarang dengan LDFLAGS untuk linking library
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@echo "Program GUI '$(TARGET)' berhasil dibuat. Jalankan dengan './$(TARGET)'"

# Target untuk menginstal program
install: all
	sudo apt install libxpm-dev m4 libx11-dev tcl-dev tk-dev libcairo2-dev mesa-common-dev libglu1-mesa-dev
	@echo "Menginstal '$(TARGET)' ke /usr/local/bin..."
	mkdir -p /usr/local/bin
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/itsopen
	cp -r ./script /usr/local/share/itsopen/
	@echo "Instalasi selesai. Anda sekarang bisa menjalankan '$(TARGET)' dari mana saja."

# Target untuk membersihkan direktori
clean:
	@echo "Membersihkan file..."
	rm -f $(TARGET)
	@echo "Selesai."

# Memberitahu 'make' bahwa 'all', 'install', dan 'clean' bukan nama file
.PHONY: all install clean
