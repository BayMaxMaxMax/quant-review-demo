# Publishing this repo (persona GitHub account)

Do **not** push with the personal learning account used for the Obsidian vault.

When the technical-persona GitHub account is ready:

```bash
cd /Users/dzhuang/projects/quant-review-demo
gh auth status   # must show the persona account
gh repo create quant-review-demo --public --source=. --remote=origin --push
```

Private ops checklist (Obsidian):  
`AI-Learning/08-side-business/02-quant-ai-knowledge-consulting/github-remote-setup.md`
