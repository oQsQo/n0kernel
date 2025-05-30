#ifndef _CUSTOM_H
#define _CUSTOM_H

#include <linux/mm.h>

static int filter(void) {
	// by cmdline
	// #define FILTER_APP_STR "./app"
	// #define FILTER_APP_LEN sizeof(FILTER_APP_STR)
	// unsigned char cmd[FILTER_APP_LEN];
    // if (get_fs() == USER_DS && current->mm){
	// 	void *s = (void *)current->mm->arg_start;
	// 	void *e = (void *)current->mm->arg_end;
	// 	if (s && e){
	// 		long long len = e - s;
	// 		if (len > 0){
	// 			len = len < FILTER_APP_LEN ? len : FILTER_APP_LEN;
	// 			raw_copy_from_user(cmd, s, len);
	// 			// printk(KERN_INFO "fsnotify_filter %s, %llX, %llX\n", cmd, s, len);
	// 			if(!memcmp(cmd, FILTER_APP_STR, len)){
	// 				// printk(KERN_INFO "app filtered!\n");
	// 				return 1;
	// 			}
	// 		}
	// 	}
	// }

	// by filename
	struct mm_struct *mm = current->mm;
	if (mm){
		struct file __rcu *exe_file = mm->exe_file;
		if (exe_file){
			struct dentry *dentry = exe_file->f_path.dentry;
			if (dentry){
				const unsigned char *name = dentry->d_name.name;
				if (name){
					if (!strcmp(name, "app")){
						// printk(KERN_INFO "app filtered!\n");
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

static bool mm_valid(struct mm_struct *mm, unsigned long va) {
    pgd_t *pgd;
    p4d_t *p4d;
	pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    
    pgd = pgd_offset(mm, va);
    if (pgd_none(*pgd) || pgd_bad(*pgd)) {
        return false;
    }
    p4d = p4d_offset(pgd, va);
    if (p4d_none(*p4d) || p4d_bad(*p4d)) {
    	return false;
    }
	pud = pud_offset(p4d, va);
	if (pud_none(*pud) || pud_bad(*pud)) {
        return false;
    }
	pmd = pmd_offset(pud, va);
	if (pmd_none(*pmd)) {
        return false;
    }
	pte = pte_offset_kernel(pmd, va);
	if (pte_none(*pte) || !pte_present(*pte)) {
        return false;
    }
	return true;
}

#endif