#pragma once

#include <stddef.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

typedef struct _stream_ops {
    int     (*open)(void *stream, void *ctx);
    int     (*close)(void *stream);
    size_t  (*read)(void *stream, void *buf, size_t size);
    size_t  (*write)(void *stream, const void *buf, size_t size);
    long    (*lseek)(void *stream, long off, int origin);
    long    (*tell)(void *stream);
} stream_ops_t;

typedef struct _stream_ctx {
    void   *buf;
    long    pos;
    long    len;
    long    size;
} stream_ctx_t;

typedef struct _file_ctx {
	int 	fd;
	long    pos;
    long    len;
} file_ctx_t;

typedef struct _stream {
    void           *ctx; // stream_ctx
    stream_ops_t   *ops;
} stream_t;


extern void *stream_ctx_bind(stream_ctx_t *ctx, void *buf, size_t size);
extern void stream_ctx_reset(stream_ctx_t *ctx);
extern stream_t *stream_init(stream_t *sp);
extern void stream_fini(stream_t *sp);

#include <string.h>

#if __STDC_HOSTED__ == 1
#include <stdlib.h>
extern stream_t *create_stream();
extern void destroy_stream(stream_t *stream_p);

extern void *create_memstream_ctx(size_t size);
extern void *resize_memstream_ctx(void *ctx, size_t new_size);
extern void destroy_memstream_ctx(void *ctx);

#endif



/*
struct file_operations {
	struct module *owner;
	fop_flags_t fop_flags;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
	ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
	ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
	int (*iopoll)(struct kiocb *kiocb, struct io_comp_batch *,
			unsigned int flags);
	int (*iterate_shared) (struct file *, struct dir_context *);
	__poll_t (*poll) (struct file *, struct poll_table_struct *);
	long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
	long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *, fl_owner_t id);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, loff_t, loff_t, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
	int (*check_flags)(int);
	int (*flock) (struct file *, int, struct file_lock *);
	ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
	ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
	void (*splice_eof)(struct file *file);
	int (*setlease)(struct file *, int, struct file_lease **, void **);
	long (*fallocate)(struct file *file, int mode, loff_t offset,
			  loff_t len);
	void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
	unsigned (*mmap_capabilities)(struct file *);
#endif
	ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
			loff_t, size_t, unsigned int);
	loff_t (*remap_file_range)(struct file *file_in, loff_t pos_in,
				   struct file *file_out, loff_t pos_out,
				   loff_t len, unsigned int remap_flags);
	int (*fadvise)(struct file *, loff_t, loff_t, int);
	int (*uring_cmd)(struct io_uring_cmd *ioucmd, unsigned int issue_flags);
	int (*uring_cmd_iopoll)(struct io_uring_cmd *, struct io_comp_batch *,
				unsigned int poll_flags);
	int (*mmap_prepare)(struct vm_area_desc *);
} __randomize_layout;

*/
