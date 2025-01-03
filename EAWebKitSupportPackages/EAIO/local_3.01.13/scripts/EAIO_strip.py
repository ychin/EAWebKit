import SimpleCodeStripper

SimpleCodeStripper.Strip(
		sourceSubdirectories = ['include', 'source', 'src', 'extras'],
		additionalSourceFoldersToSkip = ['doc'],
		folderComponentsToExcludeFromCopy=['build', 'test', 'doc'],
		)

