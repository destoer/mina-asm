#include <headers/lib.h>
#include <mina/assembler.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("usage: %s <file to assemble>\n",argv[1]);
        return 1;
    }

    if(std::string(argv[1]) == "-t")
    {
        return 0;
    }

    Assembler assembler(argv[1]);

    assembler.assemble_file();

    assembler.write_binary(get_binary_file_name(argv[1]));
}