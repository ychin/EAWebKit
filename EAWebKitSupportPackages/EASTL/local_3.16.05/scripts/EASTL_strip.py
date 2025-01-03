# Copyright (c) Electronic Arts Inc. All rights reserved.

import SimpleCodeStripper

SimpleCodeStripper.Strip(
		sourceSubdirectories = ['include', 'source', 'benchmark', 'test'],
		folderComponentsToExcludeFromCopy=['build', 'scripts', '.git'],
		filesToCopy=['./scripts/initialize.xml']
		)
