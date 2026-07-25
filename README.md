# MTK
Goal: Make an interactive shell

# Notice
This is a TOY-OS. I am using this PURELY to learn, test and understand basic kernel development. In fact, the term OS used here and anywhere within this repository is misleading based on your definition of an OS. This is much more of a kernel.

I do intend to make a real kernel and General purpose OS but this one is just not it. So what's this repo for?
* Reference: If I need any help in future
* Experiments: If I want to add a new feature and test stuff around. Say UNICODE support etc!
* Learning: To learn obv.
* Fun: Need I say more?

And before I forget, I was following a lot of references and a course from Udemy: "Build a multitasking operating system and kernel with an interactive shell! osdev" by Daniel McCarthy but **I didn't follow/copy his work 1:1 so DO NOT use this as a reference if you're stuck with that coursework**. Thanks for all the resources on the internet to help me with this and **major thanks to [OSDev](wiki.osdev.org) `<3`**!!

# References
Okay, as mentioned this is a ToyOS used purely for educational and experimental purposes. Here are all the references I used if that helps:
* [OSDev Wiki](wiki.osdev.org)
* "Build a multitasking operating system and kernel with an interactive shell! osdev" by Daniel McCarthy
* AMD developer specification volume 1-5
* Intel® 64 and IA-32 Architectures Software Developer’s Manual Combined Volumes: 1, 2A, 2B, 2C, 2D, 3A, 3B, 3C, 3D, and 4
* "Programming from the Ground Up"
* [Writing a tiny bootloader](https://www.joe-bergeron.com/posts/Writing%20a%20Tiny%20x86%20Bootloader/)
* [Ralf Brown's Interrupt List](https://www.ctyme.com/rbrown.htm)
* ELF Specification Format (You can lookup for PDFs on the internet)

A good portion of this project involves concepts not mentioned above. I became interested in systems programming at a really young age and some of the things I was already familiar with some of the concepts used here. So, I cannot provide references for them as I no longer remember where I obtained that knowledge. However, do contact me for any help, I'd be glad to help someone trying to write their own OS.

Yes, I read both AMD and Intel's architecture manuals. While they overlap to a considerable amount, it was just fun to do so and a few concepts were easy in one of the book as opposed to the other.

Read files in known_issues_fixed to know about a few issues I faced during this and how I overcame them. I've to finish writing the remaining ones.

## Warning
**A BIG WARNING:** Now, obviously its very rare that someone actually stumbles here and even more rare for someone to think "Yes, let me install this on my x86 system" but on the odd-case someone (Hmm... the person reading this..?) is thinking to do that, then simply **don't**.

I don't own a x86 system (sadly) and my computer's UEFI firmware doesn't have CSM backward compatibility which if I remember right was fully dropped in UEFI-3. As such, everything here was tested with QEMU. So yea, I don't recommend compiling it and using it.

Will it boot? Maybe.

# License
Refer to [License](LICENSE).

# Contact
* mailto: me@csral.dev
* mailto: chaturyasral@gmail.com
