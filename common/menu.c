#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdnoreturn.h>
#include <config.h>
#include <menu.h>
#include <lib/print.h>
#include <lib/misc.h>
#include <lib/libc.h>
#include <lib/config.h>
#include <lib/term.h>
#include <lib/gterm.h>
#include <lib/getchar.h>
#include <lib/uri.h>
#include <mm/pmm.h>
#include <drivers/vbe.h>
#include <drivers/vga_textmode.h>
#include <protos/linux.h>
#include <protos/chainload.h>
#include <protos/chainload_next.h>
#include <protos/multiboot1.h>
#include <protos/multiboot2.h>
#include <protos/limine.h>
#include <sys/cpu.h>
#include <lib/misc.h>

bool booting_from_editor = false;

static struct memmap_entry *rewound_memmap = NULL;
static size_t rewound_memmap_entries = 0;
static no_unwind uint8_t *rewound_data;
#if defined(BIOS)
static no_unwind uint8_t *rewound_s2_data;
static no_unwind uint8_t *rewound_bss;
#endif

extern symbol data_begin;
extern symbol data_end;
#if defined(BIOS)
extern symbol s2_data_begin;
extern symbol s2_data_end;
extern symbol bss_begin;
extern symbol bss_end;
#endif

void menu_init_memmap()
{
    size_t data_size = (uintptr_t)data_end - (uintptr_t)data_begin;
#if defined(BIOS)
    size_t s2_data_size = (uintptr_t)s2_data_end - (uintptr_t)s2_data_begin;
    size_t bss_size = (uintptr_t)bss_end - (uintptr_t)bss_begin;
#endif

    if (rewound_memmap != NULL)
    {
        memcpy(data_begin, rewound_data, data_size);
#if defined(BIOS)
        memcpy(s2_data_begin, rewound_s2_data, s2_data_size);
        memcpy(bss_begin, rewound_bss, bss_size);
#endif
        memcpy(memmap, rewound_memmap, rewound_memmap_entries * sizeof(struct memmap_entry));
        memmap_entries = rewound_memmap_entries;
    }
    else
    {
        rewound_data = ext_mem_alloc(data_size);
#if defined(BIOS)
        rewound_s2_data = ext_mem_alloc(s2_data_size);
        rewound_bss = ext_mem_alloc(bss_size);
#endif
        rewound_memmap = ext_mem_alloc(MEMMAP_MAX * sizeof(struct memmap_entry));
        if (memmap_entries > MEMMAP_MAX)
        {
            panic(false, "menu: Too many memmap entries");
        }
        memcpy(rewound_memmap, memmap, memmap_entries * sizeof(struct memmap_entry));
        rewound_memmap_entries = memmap_entries;
        memcpy(rewound_data, data_begin, data_size);
#if defined(BIOS)
        memcpy(rewound_s2_data, s2_data_begin, s2_data_size);
        memcpy(rewound_bss, bss_begin, bss_size);
#endif
    }
}

void menu_init_disks()
{
    if (bad_config == false)
    {
#if defined(UEFI)
        if (init_config_disk(boot_volume))
        {
#endif
            volume_iterate_parts(boot_volume, if (!init_config_disk(_PART)) {
                boot_volume = _PART;
                break; });
#if defined(UEFI)
        }
#endif
    }
}

void menu_init_term()
{
    char *use_gterm = config_get_value(NULL, 0, "USE_GTERM");
    if (use_gterm == NULL)
        use_gterm = "yes";

    if (strcmp(use_gterm, "yes") == 0)
    {
        size_t req_width = 0, req_height = 0, req_bpp = 0;

        char *menu_resolution = config_get_value(NULL, 0, "INTERFACE_RESOLUTION");
        if (menu_resolution != NULL)
            parse_resolution(&req_width, &req_height, &req_bpp, menu_resolution);

        if (!quiet && !gterm_init(NULL, NULL, NULL, req_width, req_height))
        {
#if defined(BIOS)
            vga_textmode_init(true);
#elif defined(UEFI)
            term_fallback();
#endif
        }
    }
    else
    {
        if (!quiet)
#if defined(BIOS)
            vga_textmode_init(true);
#elif defined(UEFI)
            term_fallback();
#endif
    }
}

char *menu_get_branding()
{
    char *menu_branding = config_get_value(NULL, 0, "INTERFACE_BRANDING");
    if (menu_branding == NULL)
    {
#if defined(BIOS)
        {
            uint32_t eax, ebx, ecx, edx;
            if (!cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx) || !(edx & (1 << 29)))
            {
                menu_branding = "Soapine " LIMINE_VERSION " (ia-32, BIOS)";
            }
            else
            {
                menu_branding = "Soapine " LIMINE_VERSION " (x86-64, BIOS)";
            }
        }
#elif defined(UEFI)
#if defined(__i386__)
        {
            uint32_t eax, ebx, ecx, edx;
            if (!cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx) || !(edx & (1 << 29)))
            {
                menu_branding = "Soapine " LIMINE_VERSION " (ia-32, UEFI32)";
            }
            else
            {
                menu_branding = "Soapine " LIMINE_VERSION " (x86-64, UEFI32)";
            }
        }
#else
        menu_branding = "Soapine " LIMINE_VERSION " ("
#if defined(__x86_64__)
                        "x86-64"
#elif defined(__riscv64)
                        "riscv64"
#elif defined(__aarch64__)
                        "aarch64"
#endif
                        ", UEFI)";
#endif
#endif
    }

    return menu_branding;
}

int get_entry_count(struct menu_entry *tree_main)
{
    // works kinda like strlen

    int menu_entry_count = 0;
    struct menu_entry *curentry = menu_tree;
    while (curentry != NULL)
    {
        menu_entry_count++;
        curentry = curentry->next;
    }
    return menu_entry_count;
}

struct menu_entry *print_menu_entries(struct menu_entry *tree_main, int sub_count, int selected_entry, int x, int y)
{
    char *selected_color = "\e[47;30m";
    char *reset = "\e[0m";

    int i = 0;
    struct menu_entry *sme = NULL;
    struct menu_entry *cme = tree_main;



    while (cme != NULL)
    {
        if (!cme->sub) {
            set_cursor_pos_helper(x, y + i);

            if (i == selected_entry)
                sme = cme;

            else
            {
                print("    ");
            }

            if (sub_count > 0)
            {
                for (int i = 0; i < sub_count; i++)
                    print("-");

                print(" ");
            }

            if (i == selected_entry)
                print(selected_color);

            print("%s\n", cme->name);

            if (i == selected_entry)
                print(reset);

            i++;
        }
        cme = cme->next;
    }

    return sme;
}

noreturn void _menu(bool first_run)
{
    menu_init_memmap();
    menu_init_disks();
    menu_init_term();

    char *menu_branding = menu_get_branding();

    struct menu_entry *selected_menu_entry = menu_tree;
    size_t selected_entry = 0;
    int max_entries = get_entry_count(menu_tree);

    char c;

refresh:
    print("\033[H\033[2J");

    set_cursor_pos_helper(1, 1);
    if (!selected_menu_entry->sub)
        print("%s - ENTER: Boot, ARROWS: Navigate", menu_branding);
    else {
        print("%s - ENTER: %s, ARROWS: Navigate", menu_branding, selected_menu_entry->expanded ? "Collapse": "Expand");
    }

    selected_menu_entry = print_menu_entries(menu_tree, 0, selected_entry, 1, 3);

    while (1)
    {
        c = getchar();

        switch (c)
        {
        case GETCHAR_CURSOR_UP:
            if (selected_entry == 0)
            {
                selected_entry = max_entries - 1;
                goto refresh;
            }
            selected_entry--;
            goto refresh;

        case GETCHAR_CURSOR_DOWN:
            if (selected_entry == max_entries - 1)
            {
                selected_entry = 0;
                goto refresh;
            }
            selected_entry++;
            goto refresh;
        case GETCHAR_CURSOR_RIGHT:
        case '\n':
        case ' ':
            if (selected_menu_entry == NULL)
                break;

            if (selected_menu_entry->sub != NULL)
            {
                selected_menu_entry->expanded = !selected_menu_entry->expanded;
                goto refresh;
            }

            if (!quiet)
            {
                if (term_backend == FALLBACK)
                {
                    if (!gterm_init(NULL, NULL, NULL, 0, 0))
                    {
#if defined(BIOS)
                        vga_textmode_init(true);
#elif defined(UEFI)
                        term_fallback();
#endif
                    }
                }
                else
                {
                    reset_term();
                }
            }

            boot(selected_menu_entry->body);

        default:
            break;
        }
    }
}

noreturn void boot(char *config)
{
    char *cmdline = config_get_value(config, 0, "KERNEL_CMDLINE");
    if (!cmdline)
    {
        cmdline = config_get_value(config, 0, "CMDLINE");
    }
    if (!cmdline)
    {
        cmdline = "";
    }

    char *proto = config_get_value(config, 0, "PROTOCOL");
    if (proto == NULL)
    {
        panic(true, "Boot protocol not specified for this entry");
    }

    if (!strcmp(proto, "limine"))
    {
        limine_load(config, cmdline);
    }
    else if (!strcmp(proto, "linux"))
    {
        linux_load(config, cmdline);
    }
    else if (!strcmp(proto, "multiboot1") || !strcmp(proto, "multiboot"))
    {
#if defined(__x86_64__) || defined(__i386__)
        multiboot1_load(config, cmdline);
#else
        quiet = false;
        print("Multiboot 1 is not available on non-x86 architectures.\n\n");
#endif
    }
    else if (!strcmp(proto, "multiboot2"))
    {
#if defined(__x86_64__) || defined(__i386__)
        multiboot2_load(config, cmdline);
#else
        quiet = false;
        print("Multiboot 2 is not available on non-x86 architectures.\n\n");
#endif
    }
    else if (!strcmp(proto, "chainload_next"))
    {
        chainload_next(config, cmdline);
#if defined(BIOS)
    }
    else if (!strcmp(proto, "bios_chainload"))
    {
#elif defined(UEFI)
    }
    else if (!strcmp(proto, "efi_chainload"))
    {
#endif
        chainload(config, cmdline);
    }

    panic(true, "Unknown protocol! Please refer to the Limine documentation for supported protocols.\n");
}