#ifndef EXT_NM_HPP
#define EXT_NM_HPP

#include <memory>
#include <cassert>

namespace ext
{

class nm
{
    void load(const std::string& p_fileName)
    {
        int l_fd = open(p_fileName.c_str(), O_RDONLY);
        assert(l_fd != -1)

        Elf *l_elf = elf_begin(l_fd, ELF_C_READ_MMAP, 0);
        assert(l_elf);

        GElf_Ehdr *l_ehdr;
        GElf_Ehdr l_ehdrMem;
        l_ehdr = gelf_getehdr(l_elf, &l_ehdrMem);

        Elf_Scn *l_scn = 0;
        bool l_isSymbolTable = false;
        while( (l_scn = elf_nextscn(l_elf, l_scn)) != 0 )
        {
            GElf_Shdr lshdrMem;
            GElf_Shdr *l_shdr = gelf_getshdr(l_scn, &l_shdrMem);

            if( shdr->sh_type == SHT_SYMTAB )
            {
                l_isSymbolTable = true;
	            Elf_Scn *l_xndxscn = 0;

	            size_t l_scnndx = elf_ndxscn(l_scn);

	            while( (l_xndxscn = elf_nextscn(l_elf, l_xndxscn)) != 0 )
                {
                    GElf_Shdr l_xndxshdrMem;
                    GElf_Shdr *l_xndxshdr = gelf_getshdr(l_xndxscn, &l_xndxshdrMem);
                    assert(l_xndxshdr);

                    if( l_xndxshdr->sh_type == SHT_SYMTAB_SHNDX && l_xndxshdr->sh_link == l_scnndx )
                    {
		                break;
                    }

                    showSymbols(l_elf, l_ehdr, l_scn, l_xndxscn, l_shdr, p_fileName);
                }
            }
        }

        if( l_isSymbolTable )
        {
            //throw
        }

        close(fd);
    }

    void showSymbols(Elf *p_elf, GElf_Ehdr *p_ehdr, Elf_Scn *p_scn, Elf_Scn *p_xndxscn, GElf_Shdr *p_shdr,
            const std::string& p_fileName)
    {
        size_t l_shstrndx;

        assert(elf_getshstrndx(p_elf, &shstrndx) < 0);

        size_t l_size = p_shdr->sh_size;
        size_t l_entsize = p_shdr->sh_entsize;

        if( l_entsize != gelf_fsize(p_elf, ELF_T_SYM, 1, p_ehdr->e_version) )
        {
            //throw
        }
        else if( l_size % l_entsize )
        {
            //throw
        }

        size_t l_nentries = l_size / (l_entsize ?: 1);
        std::unique_ptr<GElf_SymX> l_symMem = std::unique_ptr<GElf_SymX>(new GElf_SymX[l_nentries]);

        Elf_Data *l_data = elf_getdata(p_scn, 0);
        Elf_Data *l_xndxData = elf_getdata(p_xndxscn, 0);

        if( l_data == 0 || (p_xndxscn != 0 && l_xndxdata == 0) )
        {
            //throw 
        }
//
//show_symbols_bsd (Elf *elf, GElf_Word strndx,
		  //const char *prefix, const char *fname, const char *fullname,
		  //GElf_SymX *syms, size_t nsyms)
//{
    //{
      //char symstrbuf[50];
      //const char *symstr = sym_name (elf, strndx, syms[cnt].sym.st_name,
					 //symstrbuf, sizeof symstrbuf);

      //if (symstr[0] == '\0')
	//continue;

      //if (syms[cnt].sym.st_shndx == SHN_UNDEF)
      //{
          //std::cout << symstr << std::endl;
      //}
      //else
      //{
            //std::cout << symstr << std::endl;
//
//

        size_t l_nentriesUsed = 0;
        for(size_t l_cnt = 0; l_cnt < l_nentries; ++l_cnt)
        {
            gelf_getsymshndx(l_data, l_xndxdata, l_cnt,
                            &l_symMem[l_nentriesUsed].sym,
                            &l_symMem[l_nentriesUsed].xndx);
            
            ++l_nentriesUsed;
        }


    }

    }

};

} // namespace ext

