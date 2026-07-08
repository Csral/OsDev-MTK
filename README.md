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

## Warning
**A BIG WARNING:** Now, obviously its very rare that someone actually stumbles here and even more rare for someone to think "Yes, let me install this on my x86 system" but on the odd-case someone (Hmm... the person reading this..?) is thinking to do that, then simply **don't**.

I don't own a x86 system (sadly) and my computer's UEFI firmware doesn't have CSM backward compatibility which if I remember right was fully dropped in UEFI-3. As such, everything here was tested with QEMU. So yea, I don't recommend compiling it and using it.

Will it boot? Maybe.

# License
Refer to [License](LICENSE).