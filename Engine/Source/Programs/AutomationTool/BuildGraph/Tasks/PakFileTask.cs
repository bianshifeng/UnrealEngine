﻿using AutomationTool;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnrealBuildTool;

namespace BuildGraph.Tasks
{
	/// <summary>
	/// Parameters for a task that runs the cooker
	/// </summary>
	public class PakFileTaskParameters
	{
		/// <summary>
		/// Base directory to create relative paths in the pak file.
		/// </summary>
		[TaskParameter(Optional = true)]
		public string BaseDirectory;

		/// <summary>
		/// List of maps to be cooked, separated by '+' characters
		/// </summary>
		[TaskParameter]
		public string Files;

		/// <summary>
		/// PAK file to output
		/// </summary>
		[TaskParameter]
		public string Output;

		/// <summary>
		/// Directories to rebase the files relative to. If specified, the shortest path under a listed directory will be used for each file.
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Rebase;

		/// <summary>
		/// Script which gives the order of files
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Order;

		/// <summary>
		/// Encryption keys for this pak file
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Sign;

		/// <summary>
		/// Whether to compress files
		/// </summary>
		[TaskParameter(Optional = true)]
		public bool Compress = true;

		/// <summary>
		/// Additional arguments to be passed to UnrealPak
		/// </summary>
		[TaskParameter(Optional = true)]
		public string Arguments = "";
	}

	/// <summary>
	/// Cook a selection of maps for a certain platform
	/// </summary>
	[TaskElement("PakFile", typeof(PakFileTaskParameters))]
	public class PakFileTask : CustomTask
	{
		/// <summary>
		/// Parameters for the task
		/// </summary>
		PakFileTaskParameters Parameters;

		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="InParameters">Parameters for this task</param>
		public PakFileTask(PakFileTaskParameters InParameters)
		{
			Parameters = InParameters;
		}

		/// <summary>
		/// Execute the task.
		/// </summary>
		/// <param name="Job">Information about the current job</param>
		/// <param name="BuildProducts">Set of build products produced by this node.</param>
		/// <param name="TagNameToFileSet">Mapping from tag names to the set of files they include</param>
		/// <returns>True if the task succeeded</returns>
		public override bool Execute(JobContext Job, HashSet<FileReference> BuildProducts, Dictionary<string, HashSet<FileReference>> TagNameToFileSet)
		{
			DirectoryReference BaseDir = ResolveDirectory(Parameters.BaseDirectory);

			// Find the directories we're going to rebase relative to
			HashSet<DirectoryReference> RebaseDirs = new HashSet<DirectoryReference>{ BaseDir };
			if(Parameters.Rebase != null)
			{
				RebaseDirs.UnionWith(SplitDelimitedList(Parameters.Rebase).Select(x => ResolveDirectory(x)));
			}

			// Get the output parameter
			FileReference OutputFile = ResolveFile(Parameters.Output);

			// Get a unique filename for the response file
			FileReference ResponseFile = FileReference.Combine(new DirectoryReference(CommandUtils.CmdEnv.LogFolder), String.Format("PakList_{0}.txt", OutputFile.GetFileNameWithoutExtension()));
			for(int Idx = 2; ResponseFile.Exists(); Idx++)
			{
				ResponseFile = FileReference.Combine(ResponseFile.Directory, String.Format("PakList_{0}_{1}.txt", OutputFile.GetFileNameWithoutExtension(), Idx));
			}

			// Write out the response file
			HashSet<FileReference> Files = ResolveFilespec(BaseDir, SplitDelimitedList(Parameters.Files), TagNameToFileSet);
			using(StreamWriter Writer = new StreamWriter(ResponseFile.FullName, false, new System.Text.UTF8Encoding(true)))
			{
				foreach(FileReference File in Files)
				{
					string RelativePath = FindShortestRelativePath(File, RebaseDirs);
					if(RelativePath == null)
					{
						CommandUtils.LogError("Couldn't find relative path for '{0}' - not under any rebase directories", File.FullName);
						return false;
					}
					Writer.WriteLine("\"{0}\" \"{1}\"{2}", File.FullName, RelativePath, Parameters.Compress? " -compress" : "");
				}
			}

			// Format the command line
			StringBuilder CommandLine = new StringBuilder();
			CommandLine.AppendFormat("{0} -create={1}", CommandUtils.MakePathSafeToUseWithCommandLine(OutputFile.FullName), CommandUtils.MakePathSafeToUseWithCommandLine(ResponseFile.FullName));
			if(Parameters.Sign != null)
			{
				CommandLine.AppendFormat(" -sign={0}", CommandUtils.MakePathSafeToUseWithCommandLine(ResolveFile(Parameters.Sign).FullName));
			}
			if(Parameters.Order != null)
			{
				CommandLine.AppendFormat(" -order={0}", CommandUtils.MakePathSafeToUseWithCommandLine(ResolveFile(Parameters.Order).FullName));
			}
			if (GlobalCommandLine.Installed)
			{
				CommandLine.Append(" -installed");
			}
			if (GlobalCommandLine.UTF8Output)
			{
				CommandLine.AppendFormat(" -UTF8Output");
			}

			// Get the executable path
			FileReference UnrealPakExe;
			if(HostPlatform.Current.HostEditorPlatform == UnrealTargetPlatform.Win64)
			{
				UnrealPakExe = ResolveFile("Engine/Binaries/Win64/UnrealPak.exe");
			}
			else
			{
				UnrealPakExe = ResolveFile(String.Format("Engine/Binaries/{0}/UnrealPak", HostPlatform.Current.HostEditorPlatform.ToString()));
			}

			// Run it
			CommandUtils.Log("Running '{0} {1}'", CommandUtils.MakePathSafeToUseWithCommandLine(UnrealPakExe.FullName), CommandLine.ToString());
			CommandUtils.RunAndLog(CommandUtils.CmdEnv, UnrealPakExe.FullName, CommandLine.ToString(), Options: CommandUtils.ERunOptions.Default | CommandUtils.ERunOptions.UTF8Output);
			BuildProducts.Add(OutputFile);
			return true;
		}

		/// <summary>
		/// Find the shortest relative path of the given file from a set of base directories.
		/// </summary>
		/// <param name="File">Full path to a file</param>
		/// <param name="RebaseDirs">Possible base directories</param>
		/// <returns>The shortest relative path, or null if the file is not under any of them</returns>
		public static string FindShortestRelativePath(FileReference File, IEnumerable<DirectoryReference> RebaseDirs)
		{
			string RelativePath = null;
			foreach(DirectoryReference RebaseDir in RebaseDirs)
			{
				if(File.IsUnderDirectory(RebaseDir))
				{
					string NewRelativePath = File.MakeRelativeTo(RebaseDir);
					if(RelativePath == null || NewRelativePath.Length < RelativePath.Length)
					{
						RelativePath = NewRelativePath;
					}
				}
			}
			return RelativePath;
		}
	}
}
