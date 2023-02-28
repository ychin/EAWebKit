import SimpleCodeStripper

SimpleCodeStripper.Strip(
		sourceSubdirectories = ['include', 'source', 'test'],
		folderComponentsToExcludeFromCopy=['.git', 'build', 'examples'],
		)

