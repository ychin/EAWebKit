import SimpleCodeStripper

SimpleCodeStripper.Strip(
		sourceSubdirectories = ['include', 'source', 'src', 'extras', 'demo/source'],
		additionalSourceFoldersToSkip = ['doc'],
		folderComponentsToExcludeFromCopy=['build', 'test', 'doc'],
		)

