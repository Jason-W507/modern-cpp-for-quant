# Issue tracker: GitHub

Issues and PRDs for this repository live in GitHub Issues. Use the `gh` CLI from a checkout whose `origin` points at `Jason-W507/modern-cpp-for-quant`.

## Conventions

- Create: `gh issue create --title "..." --body-file <file>`.
- Read: `gh issue view <number> --comments`.
- List: `gh issue list --state open --json number,title,body,labels,assignees`.
- Comment: `gh issue comment <number> --body-file <file>`.
- Label: `gh issue edit <number> --add-label "..."` or `--remove-label "..."`.
- Close: `gh issue close <number> --comment "..."`.

Infer the repository from the local Git remote. Prefer body files for multiline Markdown so newlines and code fences render correctly.

## Pull requests as a triage surface

**PRs as a request surface: yes.** External PRs run through the same triage labels and states as Issues. Collaborator work already in progress is not treated as an incoming request.

- Read: `gh pr view <number> --comments` and `gh pr diff <number>`.
- List: `gh pr list --state open --json number,title,body,labels,author,authorAssociation,comments`.
- Treat `CONTRIBUTOR`, `FIRST_TIME_CONTRIBUTOR`, and `NONE` as external; exclude `OWNER`, `MEMBER`, and `COLLABORATOR` from request triage.
- Comment/label/close with the corresponding `gh pr` commands.

GitHub shares one number space across Issues and PRs. Resolve an ambiguous `#42` with `gh pr view 42`, then fall back to `gh issue view 42`.

## Skill operations

- When a skill says “publish to the issue tracker”, create a GitHub Issue.
- When a skill says “fetch the relevant ticket”, run `gh issue view <number> --comments`.
- Wayfinder maps and children are GitHub Issues. Use native sub-issues and issue dependencies when available; otherwise use task lists and explicit `Blocked by: #<n>` lines.
- Claim a ticket by assigning it to the driving developer; resolve it with a result comment and close it.
