# CONTRIBUTING

## Rules

- Push to master is forbidden
- Create new feature branch for your commit <https://www.atlassian.com/git/tutorials/comparing-workflows/feature-branch-workflow>
- Use simple commit message that explains what you are doing, e.g: "Add feature
  Foobar" consult <https://wiki.openstack.org/wiki/GitCommitMessages>
- Do not use to many commits for one feature
  - Squash your commits localy and then push <http://stackoverflow.com/questions/5189560/squash-my-last-x-commits-together-using-git>
- Do not bugfix and add new features in one branch, split it!
  - Allows better review process
- Your changes will be reviewed and merged by Axel Butz and Jan Stodt
- If you want to add a feature or propose something that impactes the
  project, add a Blueprint to the folder Blueprints.
  - They will get reviewed and discussed at the team meetings
- Before implimenting a feature open a Issue and assign it to you
  or the responsible person
  - When you are pushing the impemented feature,
    use in your commit message following content: *Fixes #ISSUE_ID*
  - see <https://docs.gitlab.com/ce/user/project/issues/automatic_issue_closing.html>
    for more details

### Help
- I dont know anything about git! HELP!
  - <https://rogerdudler.github.io/git-guide/index.de.html>
- I need to reset my git folder, it does stange things!
  - Follow the instructions from: <http://stackoverflow.com/a/8888015>
- I need to delete my branch
  - git branch -d the_local_branch
- I need to squash my commit into one
  - git rebase -i HEAD~NUMBER_OF_YOUR_COMMITS+1
- I want to push my commits but there is a merge conflict
  - <https://www.ralfebert.de/git/mergekonflikte-beheben/>
