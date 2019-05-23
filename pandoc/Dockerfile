FROM escalope/pandoc-mermaid-plantuml
RUN apt-get update -y \
  && apt-get install -y -o Acquire::Retries=10 --no-install-recommends \
    pandoc-citeproc
ENTRYPOINT ["pandoc","--filter", "mermaid-filter", "--filter", "pandoc-plantuml-filter"]
