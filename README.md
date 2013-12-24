Assorted Utilities
==================

Overview
--------

An assortment of tools and libraries that are too small to warrant having their own repositories.

Directory Structure
-------------------

<ul>
<li>
	<strong>/libs</strong><br />Libraries
	<ul>
		<li>libs/<strong>libsimple-base</strong> - the base library utilised by the majority of the tools and other libraries.</li>
		<li>libs/<strong>libsimplesock</strong> - lightweight socket wrapper library for working with BSD sockets under Unix-like OSes and WinSock under Windows.</li>
	</ul>
</li>
<li>
	<strong>/tools</strong><br />Tools
	<ul>
		<li>tools/<strong>compile_file</strong> - utility to create C-code byte array representations of binary files, suitable for embedding in executables.</li>
		<li>tools/<strong>mergelib</strong> - utility to merge one or more static libraries into a single output static library, utilising "libtool" under Darwin and "ar" under all other platforms.</li>
	</ul>
</li>
</ul>